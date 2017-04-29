/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */
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
/*******************************************************************************
//(C) Copyright [2009] Marvell International Ltd.
//All Rights Reserved
*******************************************************************************/

#define LOG_TAG "MarvellOverlay"

//#define _IRE
//#define _PXA168

#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hardware/overlay.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/properties.h>

#include <linux/ioctl.h> // for _IO definition
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "overlay.marvell.h"
#ifdef _PXA168
#include "pxa168fb.h"
#else
#include "pxa950fb.h"
#endif
#include "phycontmem.h"
#include "ippIP.h"
#include "gcu.h"


#ifdef _IRE
#include "ire_lib.h"
#else
#define NR_BUFFER 3 
#endif

#define MAX_QUEUE_NUM   30


#define BASELAY_DEVICE  "/dev/graphics/fb0"
#define OVERLAY2_DEVICE "/dev/graphics/fb1"
#define OVERLAY_HDMI_DEVICE "/dev/graphics/fb2"

/*Application can set below property*/
#define PROP_OVERLAY_PATH	"service.rotate.engine"
#define PROP_OVERLAY_DUMP	"overlay.dump.frame"
#define PROP_OVERLAY_32BPP	"enable.overlay.32bpp"
#define PROP_VIDEO_PATH     "service.video.path"
#define PROP_GCU_RESIZE_DISABLE	"overlay.gcu.resize.disable"

/*Property only used for IPC in overlay HAL*/
#define PROP_OVERLAY_ROTATE	"service.overlay.rotate"
#define PROP_OVERLAY_RESIZE	"service.overlay.resize"
#define PROP_ROTATE_SENSOR	"overlay.rotate.sensor"

#define OVERLAY_QUEUE_CAPACITY      NR_BUFFER

#define OVERLAY_DUMP
//#define MRVL_OVLY_DBG
#ifdef MRVL_OVLY_DBG
int flip_count=0;
#define TRACE(...) LOGD(__VA_ARGS__);
#define DUMP_QUEUE \
    { \
        TRACE("++++++++++++++++++++++++++++++DumpBufCtxQueue from %s(%d)", __FUNCTION__, __LINE__);\
        _DumpQueue(&ctx->bufCtxQueue); \
    }
#define DUMP_RELEASE_LIST \
    { \
        TRACE("++++++++++++++++++++++++++++++DumpReleaseList from %s(%d)", __FUNCTION__, __LINE__); \
        TRACE("release_list has %d elements:", ctx->releaseBufCnt); \
        overlay_buffer_header_t *dumpBufHdr; \
        for (int i = 0; i < ctx->releaseBufCnt; i++) { \
            dumpBufHdr = (overlay_buffer_header_t *)ctx->releaseBufList[i]; \
            TRACE("release_list[%d]: physicalAddr %p, virtualAddr %p", \
                 i, dumpBufHdr->paddr, dumpBufHdr->vaddr); \
        } \
    }
#else
#define TRACE(...) LOGV(__VA_ARGS__);
#define DUMP_QUEUE
#define DUMP_RELEASE_LIST
#endif

#define ALIGN16(m)  (((m + 15)/16) * 16)
#define ALIGN4(m)  (((m + 3)/4) * 4)

//#define QUEUE_TIME_MEASURE
#ifdef QUEUE_TIME_MEASURE

#include <cutils/properties.h>
#define GET_TIME_INTERVAL_USEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000) + ((((B).tv_nsec)-((A).tv_nsec))/1000))
#define GET_TIME_INTERVAL_NSEC(A, B) (((((B).tv_sec)-((A).tv_sec))*1000000000LL) + ((((B).tv_nsec)-((A).tv_nsec))))

struct timespec overlay_time[10];
static long  time_total[10];

#if 1	
void caculate_delta_time(struct timespec oldtime, struct timespec newtime){

	clock_gettime(CLOCK_REALTIME, &newtime);

	if((newtime.tv_sec != 0) || (newtime.tv_nsec != 0)){
 	  	LOGD("delta time between oldtime and newtime is %ldus",((GET_TIME_INTERVAL_USEC(oldtime, newtime )))  );
// 	   	time_total[j] += ((GET_TIME_INTERVAL_USEC(oldtime, newtime )))  ;
 	}
}
#else
void caculate_delta_time(struct timespec time[i], struct timespec time[j]){

	clock_gettime(CLOCK_REALTIME, &time[j]);

	if((time[j].tv_sec != 0) || (time[j].tv_nsec != 0)){
 	  	LOGD("delta time between time[%d] and time[%d]=%ldus",i,j,((GET_TIME_INTERVAL_USEC(time[i], time[j] )))  );
 	   	time_total[j] += ((GET_TIME_INTERVAL_USEC(time[i], time[j] )))  ;
 	}
}

#endif

#endif

#ifdef OVERLAY_DUMP
void uyvy_dump_frame_buffer(int counter,void * buf, unsigned width, unsigned height)
{
	char value[PROPERTY_VALUE_MAX];
	property_get(PROP_OVERLAY_DUMP, value, "0");

    if ( strcmp(value, "1") == 0){
        char fname[100];
        sprintf(fname,"/data/overlay_dump.uyvy");
        LOGE("dump overlay buf as UYVY");
        FILE *fp_video = fopen(fname, "ab");
        unsigned int len = width * height * 2;

        if (fp_video) {
                   LOGE(" fwrite %d bytes image contect to fp_video, buf = %p", len, buf);
                   LOGE(" first three byte: %x, %x, %x", ((char*)buf)[0], ((char*)buf)[1],  ((char*)buf)[2]);
                   fwrite((char *)buf,               1,  len,       fp_video);
        }
           else
           {
                   LOGE("fp_video open failed");
                   return ;
           }
    
        fclose(fp_video);
	}

    return ;
}

void rgb565_dump_frame_buffer(int counter,void * buf, unsigned width, unsigned height)
{
	char value[PROPERTY_VALUE_MAX];
	property_get(PROP_OVERLAY_DUMP, value, "0");
    
	if ( strcmp(value, "1") == 0){
        char fname[100];
        sprintf(fname,"/data/overlay_dump.rgb565");
        LOGE("dump overlay buf as RGB565");
 
        FILE *fp_video = fopen(fname, "ab");
        unsigned int len = width * height * 2;
 
        if (fp_video) {
			LOGE(" fwrite %d bytes image contect to fp_video, buf = %p", len, buf);
			LOGE(" first three byte: %x, %x, %x", ((char*)buf)[0], ((char*)buf)[1],  ((char*)buf)[2]);
			fwrite((char *)buf,               1,  len,       fp_video);
        }
        else{
			LOGE("fp_video open failed");
            return ;
        }
 
        fclose(fp_video);
	}

    return ;
}

void yuv420_dump_frame_buffer(int counter,void * buf, unsigned width, unsigned height)
{
	char value[PROPERTY_VALUE_MAX];
	property_get(PROP_OVERLAY_DUMP, value, "0");
	if ( strcmp(value, "1") == 0){
        char fname[100];
        sprintf(fname,"/data/overlay_dump.yuv420");
        LOGE("dump overlay buf as YUV420");
        FILE *fp_video = fopen(fname, "ab");
		unsigned int ylen = width * height *3/2;
    
        if (fp_video) {
			LOGE(" fwrite %d bytes image contect to fp_video, buf = %p", ylen, buf);
			LOGE(" first three byte: %x, %x, %x", ((char*)buf)[0], ((char*)buf)[1],  ((char*)buf)[2]);
			fwrite(buf,               1,  ylen,       fp_video);
        }
		else {
			LOGE("fp_video open failed");
			return ;
		}
    
		fclose(fp_video);
	}

    return ;
}

#define UYVY_DUMP(x) uyvy_dump_frame_buffer(x, (void *)bufctx->bufHdr.vaddr, datactx->width, datactx->height);
#define RGB565_DUMP(x) rgb565_dump_frame_buffer(x, (void *)bufctx->bufHdr.vaddr, datactx->width, datactx->height);
#define YUV420_DUMP(x) yuv420_dump_frame_buffer(x, (void *)bufctx->bufHdr.vaddr, datactx->width, datactx->height);
#else

void uyvy_dump_frame_buffer(int counter,void * buf, unsigned width, unsigned height){};
void rgb565_dump_frame_buffer(int counter,void * buf, unsigned width, unsigned height){};
void yuv420_dump_frame_buffer(int counter,void * buf, unsigned width, unsigned height){};

#endif 

typedef enum{
	ROT_DEGREE_0,
	ROT_DEGREE_90,
	ROT_DEGREE_180,
	ROT_DEGREE_270
}ROTATION_DEGREE;

/*****************************************************************************/

struct overlay_queue_t {
    void **pData;
    int iCapacity;
    int iDeQueuePos;
    int iQueueCount;
};

struct overlay_buffer_context_t {
    struct overlay_buffer_header_t  bufHdr;
    /* our private state goes below here */
    int    index;
    void   *paddr_int_buf;  // physical address of intermediate buffer for rotate or other processing
    void   *vaddr_int_buf;  // virtual address of intermediate buffer for rotate or other processing
    void   *pstart;
};

struct overlay_control_context_t {
    struct overlay_control_device_t device;
    /* our private state goes below here */
};

struct overlay_data_context_t {
    struct overlay_data_device_t device;
    /* our private state goes below here */
    int                   fd_ovly;
    FBVideoMode           format;
    int                   width;
    int                   height;
    int                   zoom_width;
    int                   zoom_height;

    int                   baselay_width;
    int                   baselay_height;

    int                   transform;   // 0:normal; others:OVERLAY_TRANSFORM_xxx
    int                   resize;   // 0:normal; 1: do resize for the video frame in overlay
    int                   colorspace_convert; //0: no convert; 1: 420sp->420p
#ifdef _IRE
    struct ire_info       ireHandle;
#endif
	GCUContext 			  gcuContext;
	GCUFence				gcuFence[2];
	overlay_buffer_context_t  *cur_ctx;
	overlay_buffer_context_t  *last_ctx;
	int 					flag;
	int 					rotate_engine;

	unsigned char         *phycontHeap;    // Physical continuous memory heap for IRE dest buffer pool

    overlay_buffer_context_t  *used_buf_ctx_array[OVERLAY_QUEUE_CAPACITY];
    overlay_buffer_context_t  *free_buf_ctx_array[OVERLAY_QUEUE_CAPACITY];
    overlay_buffer_context_t  *buf_ctx_pool;      // overlay buffer contexts

    bool                      ovlyOn;           // 0: ovly DMA is off, 1: ovly DMA is on
    struct _sOvlySurface OvlySurface;           // To avoid mismatch between Surface and the rotate/resize dst buffer data,
												// we need to get the Surface info before rotate/resize operation
												// and pass it to LCD driver after rotate/resize.
    overlay_buffer_context_t  colorconvert_buf_ctx;   //internal buf added for color space convert
};

static int overlay_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device);

static struct hw_module_methods_t overlay_module_methods = {
    open: overlay_device_open
};

struct overlay_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: OVERLAY_HARDWARE_MODULE_ID,
        name: "MARVELL Sample Overlay module",
        author: "MARVELL",
        methods: &overlay_module_methods,
    }
};

/*****************************************************************************/

/*
 * This is the overlay_t object, it is returned to the user and represents
 * an overlay.
 * This handles will be passed across processes and possibly given to other
 * HAL modules (for instance video decode modules).
 */

struct handle_t : public native_handle {
    /* add the data fields we need here, for instance: */
    int         fd_ovly;
    FBVideoMode format;
    uint32_t    width;
    uint32_t    height;
    uint32_t    zoom_width;
    uint32_t    zoom_height;
    int         offset_x;
    int         offset_y;
    int         transform;
    int         resize;
    bool      b_finput420sp; //added  for 420sp reserve
    _sOvlySurface surface;
    bool commit_flag;
};

class overlay_object : public overlay_t {
    static overlay_handle_t getHandleRef(struct overlay_t* overlay) {
        /* returns a reference to the handle, caller doesn't take ownership */
        return &((static_cast<overlay_object *>(overlay))->mHandle);
    }

public:
    overlay_object(int fd_ovly, FBVideoMode format, int w, int h, int zoom_w, int zoom_h,
                   int x, int y, int transform,  int resize, _sOvlySurface surface, bool flag, uint32_t baselay_w, uint32_t baselay_h, bool finput420sp)
    {
        this->overlay_t::getHandleRef = getHandleRef;
        this->overlay_t::w = w;
        this->overlay_t::h = h;
        this->overlay_t::format = format;
	
        mHandle.version = sizeof(native_handle);
        mHandle.numFds      = 1;
        mHandle.numInts     = (sizeof(handle_t)-sizeof(native_handle))/sizeof(int)-1; //8; // extra ints we have in our handle
        mHandle.fd_ovly     = fd_ovly;
        mHandle.format      = format;
        mHandle.width       = w;
        mHandle.height      = h;
        mHandle.zoom_width  = zoom_w;
        mHandle.zoom_height = zoom_h;
        mHandle.offset_x    = x;
        mHandle.offset_y    = y;
        mHandle.transform   = transform;
        mHandle.resize   = resize;
	 mHandle.b_finput420sp = finput420sp;
	 mHandle.surface  = surface;
	 mHandle.commit_flag = flag;
        baselay_width       = baselay_w;
		baselay_height      = baselay_h;
    }

    handle_t mHandle;
    uint32_t baselay_width;
    uint32_t baselay_height;
};

// ****************************************************************************
// Local Functions
// ****************************************************************************
void _DeArray(struct overlay_buffer_context_t **parray, struct overlay_buffer_context_t **ppelement, int array_size)
{
    int i;

    //TRACE("%s e, array_size = %d", __FUNCTION__, array_size);
    *ppelement = NULL;
    for (i = 0; i < array_size; i++) {
	//TRACE("%s: parray[%d] = %p\n", __FUNCTION__, i, parray[i]);
	if (parray[i]) {
	    //TRACE("%s: find a free buffer header %p\n", __FUNCTION__, parray[i]);
	    *ppelement = parray[i];
	    parray[i]  = NULL;
	    break;
	}
    }
    //TRACE("%s: *ppelement = %p\n", __FUNCTION__, *ppelement);

    //TRACE("%s x", __FUNCTION__);

}

void _EnArray(struct overlay_buffer_context_t **parray, struct overlay_buffer_context_t *pelement, int array_size)
{
    int i;
    //TRACE("%s e", __FUNCTION__);
    for (i = 0; i < array_size; i++) {
	//TRACE("%s: parray[%d] = %p\n", __FUNCTION__, i, parray[i]);
	if (NULL == parray[i]) {
	    //TRACE("%s: insert a free buffer header %p\n", __FUNCTION__, pelement);
	    parray[i] = pelement;
	    break;
	}
    }
    //TRACE("%s x", __FUNCTION__);
}

#ifdef _IRE
static int _overlay_ire_open(struct overlay_data_context_t *ctx)
{
    TRACE("%s e", __FUNCTION__);
	struct ire_info *hIRE = &ctx->ireHandle;
    struct v_frame vsrc, vdst[OVERLAY_QUEUE_CAPACITY];
    int    dst_width, dst_height;
    int    dst_length[3];

    vsrc.width  = ctx->width;
    vsrc.height = ctx->height;

    ire_set_use_srcbuf(hIRE, 1);
    hIRE->ctx = 0;

    if (ire_open(hIRE, &vsrc) < 0) {
        LOGE("%s: IRE open error", __FUNCTION__);
        return -EINVAL;
    }

	TRACE("IRE has been opened");

    TRACE("%s x", __FUNCTION__);
    return 0;
}


static int _overlay_ire_close(struct overlay_data_context_t *ctx)
{
    TRACE("%s e", __FUNCTION__);

    ire_close(&ctx->ireHandle);

    TRACE("IRE has been closed");
    TRACE("%s x", __FUNCTION__);
    return 0;
}

IRE_LIB_ROTATION get_ire_rotate_degree(int rotate) {
    TRACE("%s e", __FUNCTION__);
	IRE_LIB_ROTATION ire_rotation = IRE_LIB_ROT_90;
    TRACE("%s rotate = %d", __FUNCTION__, rotate);

	switch (rotate){
		case 0:
			ire_rotation = IRE_LIB_ROT_0;
			break;

		case 90:
			ire_rotation = IRE_LIB_ROT_90;
			break;

		case 180:
			/* We do not support 180 degree rotation for YUV420 format currently */
			ire_rotation = IRE_LIB_ROT_0;
			break;

		case 270:
			ire_rotation = IRE_LIB_ROT_270;
			break;

		default:
			break;
	}

    TRACE("%s: rotate = %d, ire_rotation = %d", __FUNCTION__, rotate, ire_rotation);
    TRACE("%s x", __FUNCTION__);
    return ire_rotation;
}



static int _overlay_ire_process(struct overlay_data_context_t *ctx,
                                    struct overlay_buffer_context_t *bufctx)
{
    TRACE("%s e", __FUNCTION__);
    struct v_frame vsrc, vdst;

	ctx->ireHandle.rotate_degree	= get_ire_rotate_degree(ctx->transform);

	//In case ROI information is passed, we need to rotate the buffer, which is larger than the actual frame data.
	vsrc.width     = bufctx->bufHdr.stride_x ;
	vsrc.height    = bufctx->bufHdr.stride_y;

    vsrc.start[0]  = bufctx->bufHdr.vaddr;
    vsrc.length[0] = bufctx->bufHdr.len;

	//only support 90/270 degree rotation
    vdst.width     = vsrc.height;
    vdst.height    = vsrc.width;

    vdst.start[0]  = bufctx->vaddr_int_buf;
    vdst.length[0] = bufctx->bufHdr.len;
	
	TRACE("%s IRE rotate %d degree ", __FUNCTION__, ctx->ireHandle.rotate_degree);
    TRACE("%s IRE src buf: (%d)x(%d), address: %p, length = %d ", __FUNCTION__, vsrc.width, vsrc.height, vsrc.start[0],vsrc.length[0]);
    TRACE("%s IRE dst buf: (%d)x(%d), address: %p, length = %d ", __FUNCTION__, vdst.width, vdst.height, vdst.start[0],vdst.length[0]);

    ire_process(&ctx->ireHandle, &vsrc, &vdst);

	//set the video output format to 420P
	ctx->OvlySurface.videoMode= FB_VMODE_YUV420PLANAR;

	yuv420_dump_frame_buffer(420, (void *)bufctx->vaddr_int_buf, bufctx->bufHdr.stride_x, bufctx->bufHdr.stride_y);

    TRACE("%s x", __FUNCTION__);
    return 0;
}
#else
static int _overlay_ire_open(struct overlay_data_context_t *ctx)
{
    TRACE("%s e", __FUNCTION__);
	LOGE(" IRE is not supported!");
    TRACE("%s x", __FUNCTION__);
	return 0;
}


static int _overlay_ire_close(struct overlay_data_context_t *ctx)
{
    TRACE("%s e", __FUNCTION__);
	LOGE(" IRE is not supported!");
    TRACE("%s x", __FUNCTION__);
    return 0;
}



static int _overlay_ire_process(struct overlay_data_context_t *ctx,
                                    struct overlay_buffer_context_t *bufCtx)
{
    TRACE("%s e", __FUNCTION__);
	LOGE(" IRE is not supported!");
    TRACE("%s x", __FUNCTION__);
    return 0;
}

#endif


static int gcu_init(struct overlay_data_context_t *datactx){
    TRACE("%s e", __FUNCTION__);

	GCUenum result;

	GCU_INIT_DATA initData;
	GCU_CONTEXT_DATA contextData;

    // Init GCU library
    memset(&initData, 0, sizeof(initData));
  	gcuInitialize(&initData);

    // Create Context
	memset(&contextData, 0, sizeof(contextData));
	datactx->gcuContext = gcuCreateContext(&contextData);
	if(datactx->gcuContext == NULL)
	{
		result = gcuGetError();
		exit(0);
	}
	
	datactx->gcuFence[0] = gcuCreateFence(datactx->gcuContext);
	datactx->gcuFence[1] = gcuCreateFence(datactx->gcuContext);
    TRACE("%s x", __FUNCTION__);
	return 0;
}

static int gcu_exit(struct overlay_data_context_t *datactx){
    TRACE("%s e", __FUNCTION__);

	gcuDestroyFence(datactx->gcuContext, datactx->gcuFence[0]);
	gcuDestroyFence(datactx->gcuContext, datactx->gcuFence[1]);
	gcuDestroyContext(datactx->gcuContext);
	gcuTerminate();

    TRACE("%s x", __FUNCTION__);
	return 0;
}

GCU_ROTATION get_gcu_rotate_degree(int rotate) {
    TRACE("%s e", __FUNCTION__);
    GCU_ROTATION gcu_rotation = GCU_ROTATION_90;
    TRACE("%s rotate = %d", __FUNCTION__, rotate);

	switch (rotate){
		case 0:
			gcu_rotation = GCU_ROTATION_0;
			break;

		case 90:
			gcu_rotation = GCU_ROTATION_90;
			break;

		case 180:
			gcu_rotation = GCU_ROTATION_180;
			break;

		case 270:
			gcu_rotation = GCU_ROTATION_270;
			break;

		default:
			break;
	}

    TRACE("%s: rotate = %d, gcu_rotation = %d", __FUNCTION__, rotate, gcu_rotation);
    TRACE("%s x", __FUNCTION__);
    return gcu_rotation;
}

static int _ipp_colorspace_convert_process(
	struct overlay_data_context_t *datactx,
	struct overlay_buffer_context_t *bufctx )
{
    TRACE("%s e", __FUNCTION__);
    //convert from "bufctx->bufHdr.vaddr" to "datactx->colorconvert_buf_ctx.bufHdr.vaddr"
    if(datactx->colorspace_convert )
    {   //current the overly doesn't support 420sp, so we need to transform it to 420p.
        if(datactx->format ==FB_VMODE_YUV420PLANAR )
		{ //current ipp only support 420sp to 420p.
			 int ysize=(datactx->height) * (datactx->width);
		 	 int	 ret;
			 Ipp8u *pSrc[2], *pDst[3], *working_buf;
			 int srcStep[2], dstStep[3];
			 IppiSize  roiSize;
			
		        if(!datactx->colorconvert_buf_ctx.bufHdr.paddr || !datactx->colorconvert_buf_ctx.bufHdr.vaddr) {
		              //check if color convert buf available.
			       LOGE("%s: No available internal buf for color convert!", __FUNCTION__);
			   	return -EINVAL;
		        }
    
			 pSrc[0] = (Ipp8u*)(bufctx->bufHdr.vaddr);
			 pSrc[1] = pSrc[0]+ysize; //should be pSrc[1]+height*step0;

			//Allocate the working buffer
			 working_buf = (Ipp8u *)malloc(ysize * 3/2);
			 pDst[0] = working_buf;
			 pDst[1] = pDst[0]+ysize; //FIX ME: need to consider the step here??
			 pDst[2] = pDst[1]+(ysize>>2); //FIX ME: need to consider the step here??
  
			 
			//Set pDst[0] buffer the same as pSrc[0] to save Y component memory copy during color convert;
			 pDst[0] = pSrc[0];
			 	
			srcStep[0] = bufctx->bufHdr.step0;
			srcStep[1] = bufctx->bufHdr.step1;
			dstStep[0] = bufctx->bufHdr.step0;
			dstStep[1] = dstStep[2] = srcStep[1]>>1; //FIX ME: need to define step conception in Overlay
			roiSize.height = datactx->height;
			roiSize.width = datactx->width;
    
			ret = ippiYCbCr420SPToYCbCr420P_8u_P2P3R( (const Ipp8u**)pSrc, srcStep, pDst, dstStep, roiSize );
			if( ret != ippStsNoErr ){
				free(working_buf);
				LOGE("%s: ippiYCbCr420SPToYCbCr420P_8u_P2P3R return error", __FUNCTION__);
			   	return -EINVAL;
			}

			// copy the UV component back to src buffer
			 memcpy(pSrc[1], pDst[2], (ysize >>2));
			 memcpy(((Ipp8u *)pSrc[1] + (ysize >>2)), pDst[1], (ysize >>2));
			
			free(working_buf);
			
        }else{
			LOGE("%s: Doesn't support to transfer 420sp to format(%d)", __FUNCTION__, datactx->format);
        }
		
    }
    TRACE("%s x", __FUNCTION__);
    return 0;    
}

static int gcu_yuv_to_rgb_rotate(
		struct overlay_data_context_t *datactx,
		struct overlay_buffer_context_t *bufctx)
{
    TRACE("%s e", __FUNCTION__);


#ifdef QUEUE_TIME_MEASURE
	caculate_delta_time(overlay_time[2], overlay_time[6]);
#endif	

	GCUContext pContext 		= datactx->gcuContext;
	GCU_ROTATION rotation		= get_gcu_rotate_degree(datactx->transform);

	GCU_BLT_DATA bltData;

	/* set the Source Buffer information for GCU rotate/resize operation */
	/*  Source GCU Surface*/
	GCUSurface pSrcSurface = NULL;
	GCUVirtualAddr SrcVirtAddr  = (GCUVirtualAddr)bufctx->bufHdr.vaddr;
	GCUPhysicalAddr SrcPhysAddr = (GCUPhysicalAddr)bufctx->bufHdr.paddr;
	int srcbuf_width			= bufctx->bufHdr.stride_x ;
	int srcbuf_height			= bufctx->bufHdr.stride_y;

	GCU_FORMAT src_surface_format, dst_surface_format;

    switch (datactx->format) {
 	/*PXA910*/
    case FB_VMODE_YUV420PLANAR:
		src_surface_format = GCU_FORMAT_I420;
		break;
 	/*PXA950*/
    case FB_VMODE_YUV422PACKED:
		src_surface_format = GCU_FORMAT_UYVY;
	 	break;
    default:
        LOGE("%s: ERROR: GCU only support 420PLANAR and 422PACKED format (err fromat %x)", __FUNCTION__,datactx->OvlySurface.videoMode);
        return -EINVAL;
    }

	char value[PROPERTY_VALUE_MAX];
	property_get(PROP_OVERLAY_32BPP, value, "0");
	if ( strcmp(value, "1") == 0){
		dst_surface_format = GCU_FORMAT_ARGB8888;
	}
	else{
		dst_surface_format = GCU_FORMAT_RGB565;
	}	

	/* Source GCU subrectangle */
	GCU_RECT src_subrect;
	int srcwidth				= datactx->width;
	int srcheight				= datactx->height;

	src_subrect.left			= bufctx->bufHdr.x_off;
	src_subrect.right			= bufctx->bufHdr.x_off + srcwidth ;
	src_subrect.top				= bufctx->bufHdr.y_off;
	src_subrect.bottom			= bufctx->bufHdr.y_off + srcheight ;
	/* Dst GCU Surface */
	GCUSurface pDstSurface = NULL;
	GCUVirtualAddr DstVirtAddr  = (GCUVirtualAddr)bufctx->vaddr_int_buf;
	GCUPhysicalAddr DstPhysAddr	= (GCUPhysicalAddr)bufctx->paddr_int_buf;

	int dstwidth				= ALIGN16(datactx->OvlySurface.viewPortInfo.zoomXSize);
	int dstheight				= ALIGN4(datactx->OvlySurface.viewPortInfo.zoomYSize);

	datactx->OvlySurface.viewPortInfo.srcWidth	= dstwidth ;
	datactx->OvlySurface.viewPortInfo.srcHeight	= dstheight ;

	TRACE("%s srcwidth	= %d, srcheight	= %d ", __FUNCTION__,  srcwidth,srcheight);
	TRACE("%s rotation	= %d", __FUNCTION__,rotation);
	
	TRACE("%s SrcVirtAddr 	= %p, SrcPhysAddr	= %p, DstVirtAddr= %p, DstPhysAddr = %p  ", 
		__FUNCTION__, SrcVirtAddr,  SrcPhysAddr ,DstVirtAddr ,DstPhysAddr);

	TRACE("%s Src buffer infomation: x_off= %d, y_off = %d, stride_x = %d, stride_y = %d", 
		__FUNCTION__, bufctx->bufHdr.x_off, bufctx->bufHdr.y_off, bufctx->bufHdr.stride_x, bufctx->bufHdr.stride_y);

	TRACE("%s Src rectangle: left= %d, right = %d, top = %d, bottom = %d", 
		__FUNCTION__, src_subrect.left, src_subrect.right, src_subrect.top, src_subrect.bottom);
	
	TRACE("%s dstwidth	= %d, dstheight	= %d ", __FUNCTION__,  dstwidth,dstheight);

	pSrcSurface = _gcuCreatePreAllocBuffer(pContext,srcbuf_width , srcbuf_height, src_surface_format , GCU_TRUE, SrcVirtAddr, GCU_TRUE, SrcPhysAddr);
	pDstSurface = _gcuCreatePreAllocBuffer(pContext, dstwidth, dstheight, dst_surface_format , GCU_TRUE, DstVirtAddr, GCU_TRUE, DstPhysAddr);

	if(pSrcSurface && pDstSurface){
		memset(&bltData, 0, sizeof(bltData));
		bltData.pSrcSurface			= pSrcSurface;
		bltData.pDstSurface			= pDstSurface;
		bltData.pSrcRect		= &src_subrect;
		bltData.rotation = rotation;
		gcuBlit(pContext, &bltData);

#ifdef QUEUE_TIME_MEASURE
	caculate_delta_time(overlay_time[6], overlay_time[7]);
	//clock_gettime(CLOCK_REALTIME, &queue_time7);
 	//if((queue_time7.tv_sec != 0) || (queue_time7.tv_nsec != 0)){
 	//  	LOGD("time7=%ldus", ((GET_TIME_INTERVAL_USEC(queue_time6, queue_time7 )))  );
 	//   	queue_time7_total += ((GET_TIME_INTERVAL_USEC(queue_time6, queue_time7 )))  ;
 	//}
#endif

		gcuFinish(pContext);

#ifdef QUEUE_TIME_MEASURE
	caculate_delta_time(overlay_time[7], overlay_time[8]);
	//clock_gettime(CLOCK_REALTIME, &queue_time8);
 	//if((queue_time8.tv_sec != 0) || (queue_time8.tv_nsec != 0)){
 	//  	LOGD(" time8=%ldus", ((GET_TIME_INTERVAL_USEC(queue_time7, queue_time8 )))  );
 	//   	queue_time8_total += ((GET_TIME_INTERVAL_USEC(queue_time7, queue_time8 )))  ;
 	//}
#endif

#ifdef OVERLAY_DUMP
	property_get(PROP_OVERLAY_DUMP, value, "0");
	if ( strcmp(value, "1") == 0){
		static int i=0;
		LOGE("dump to /data/overlay_dump%d.bmp", i);
		_gcuDumpSurface(pContext, pDstSurface, "/data/overlay_dump");
		TRACE("dump Surface %d : %s: format(%x), %dx%d -> %dx%d, (%d, %d)",
			i, __FUNCTION__, datactx->OvlySurface.videoMode,
			datactx->OvlySurface.viewPortInfo.srcWidth, datactx->OvlySurface.viewPortInfo.srcHeight,
			datactx->OvlySurface.viewPortInfo.zoomXSize, datactx->OvlySurface.viewPortInfo.zoomYSize,
			datactx->OvlySurface.viewPortOffset.xOffset, datactx->OvlySurface.viewPortOffset.yOffset);
			i++;
	}

#endif
    }

    if(pSrcSurface){
	    _gcuDestroyBuffer(pContext, pSrcSurface);
    }

    if(pDstSurface){
      _gcuDestroyBuffer(pContext, pDstSurface);
    }

#ifdef QUEUE_TIME_MEASURE
	caculate_delta_time(overlay_time[8], overlay_time[9]);
#endif

	int bytes_per_pixel = 2;;

	switch(dst_surface_format){
		case GCU_FORMAT_RGB565:
			//set the video output format
			TRACE("%s, use GCU_FORMAT_RGB565 as output format", __FUNCTION__);
			datactx->OvlySurface.videoMode= FB_VMODE_RGB565;
			bytes_per_pixel = 2;;
			break;

		case GCU_FORMAT_ARGB8888:
			//set the video output format
			TRACE("%s, use GCU_FORMAT_ARGB8888 as output format", __FUNCTION__);
			datactx->OvlySurface.videoMode= FB_VMODE_RGBA888;
			bytes_per_pixel = 4;;
			break;

		default:
			LOGE("Invalid GCU output format setting! Only support GCU_FORMAT_RGB565 and GCU_FORMAT_RGBA8888.");
			break;
	}

#ifdef _PXA168
	datactx->OvlySurface.viewPortInfo.yPitch =  datactx->OvlySurface.viewPortInfo.srcWidth * bytes_per_pixel;
	datactx->OvlySurface.viewPortInfo.uPitch = 0 ;
	datactx->OvlySurface.viewPortInfo.vPitch = 0 ;
#else //PXA950
	datactx->OvlySurface.viewPortInfo.ycPitch =  datactx->OvlySurface.viewPortInfo.srcWidth * bytes_per_pixel;
	datactx->OvlySurface.viewPortInfo.uvPitch = 0;
#endif

    TRACE("%s x", __FUNCTION__);
	return 0;
}

IppCameraRotation get_ipp_rotate_degree(int rotate) {
    TRACE("%s e", __FUNCTION__);
	IppCameraRotation  ipp_rotation =  ippCameraRotate90R;
    TRACE("%s rotate = %d", __FUNCTION__, rotate);

	switch (rotate){
		case 0:
			ipp_rotation = ippCameraRotateDisable;
			break;

		case 90:
			ipp_rotation = ippCameraRotate90R;
			break;

		case 180:
			/* We do not support 180 degree rotation for YUV420 format currently */
			ipp_rotation = ippCameraRotate180;
			break;

		case 270:
			ipp_rotation = ippCameraRotate90L;
			break;

		default:
			break;
	}

    TRACE("%s: rotate = %d, ipp_rotation = %d", __FUNCTION__, rotate, ipp_rotation);
    TRACE("%s x", __FUNCTION__);
    return ipp_rotation;
}


static int ipp_yuv_rotate(struct overlay_data_context_t *datactx, 
                                struct overlay_buffer_context_t *bufctx)
{
    TRACE("%s e", __FUNCTION__);

    const Ipp8u *pSrc[3];
    int srcStep[3];
    IppiSize srcSize;
    Ipp8u *pDst[3];
    int dstStep[3];
    IppiSize dstSize;
    IppCameraInterpolation interpolation;
    IppCameraRotation rotation;
    int rcpRatiox;
    int rcpRatioy;
	int width, height;

	width = bufctx->bufHdr.stride_x;
	height = bufctx->bufHdr.stride_y;

	TRACE("%s, width = %d, height = %d",__FUNCTION__, width, height);

    if (FB_VMODE_YUV420PLANAR == datactx->format) {
        pSrc[0]         = (Ipp8u*)bufctx->bufHdr.vaddr;
        pSrc[1]         = pSrc[0] + width * height;
        pSrc[2]         = pSrc[1] + width * height / 4;
        srcStep[0]      = width;
        srcStep[1]      = width / 2;
        srcStep[2]      = width / 2;

        srcSize.width   = width;
        srcSize.height  = height;

        pDst[0]         = (Ipp8u*)bufctx->vaddr_int_buf;
        pDst[1]         = pDst[0] + width * height;
        pDst[2]         = pDst[1] + width * height / 4;

        rotation        = get_ipp_rotate_degree(datactx->transform);

		if(ippCameraRotate90R == rotation || ippCameraRotate90L == rotation)
        	dstStep[0]      = height;
		else 
	        dstStep[0]      = width;

        dstStep[1]      = dstStep[0] / 2;
        dstStep[2]      = dstStep[0] / 2;

        dstSize.width   = width;
        dstSize.height  = height;

        interpolation   = ippCameraInterpNearest;

        rcpRatiox       = 65536;
        rcpRatioy       = 65536;

		ippiYCbCr420RszRot_8u_P3R(pSrc, srcStep, srcSize, pDst, dstStep, dstSize,
            interpolation, rotation, rcpRatiox, rcpRatioy);
	
		//set the video output format to 420P
		datactx->OvlySurface.videoMode= FB_VMODE_YUV420PLANAR;

		yuv420_dump_frame_buffer(420, (void *)bufctx->vaddr_int_buf, bufctx->bufHdr.stride_x, bufctx->bufHdr.stride_y);

	}
	else if (FB_VMODE_YUV422PACKED == datactx->format) {

		const Ipp8u *pSrc[3];
        int srcStep[3];
        IppiSize srcSize;
        Ipp8u *pDst[3];
        int dstStep[3];
        IppiSize dstSize;
        IppCameraCsc colorConversion;
        IppCameraInterpolation interpolation;
        IppCameraRotation rotation;
        int rcpRatiox;
        int rcpRatioy;

		assert( bufctx->bufHdr.vaddr );

        pSrc[0]         = (Ipp8u*)bufctx->bufHdr.vaddr;
		srcStep[0]      = width * 2;
        srcSize.width   = width;
        srcSize.height  = height;

        pDst[0]         = (Ipp8u*)bufctx->vaddr_int_buf;
        pDst[1]         = pDst[0] + width * height;
        pDst[2]         = pDst[1] + width * height / 4;

        dstStep[0]      = height;
        dstStep[1]      = height / 2;
        dstStep[2]      = height / 2;

        rotation        = get_ipp_rotate_degree(datactx->transform);
//        rotation        = ippCameraRotate90R;

		ippiYCbCr422ToYCbCr420Rotate_8u_C2P3R( pSrc[0], srcStep[0], pDst,  dstStep, srcSize, rotation);
	
		//set the video output format to UYVY
		datactx->OvlySurface.videoMode= FB_VMODE_YUV422PACKED ;

		uyvy_dump_frame_buffer(422, (void *)bufctx->vaddr_int_buf, bufctx->bufHdr.stride_x, bufctx->bufHdr.stride_y);


	}

    TRACE("%s x", __FUNCTION__);
    return 0;
}

static int non_zero_start_point_process(struct overlay_data_context_t *datactx,
                                struct overlay_buffer_context_t *bufctx)
{
 	   // new start point after rotation/resize;
		int	x_off = bufctx->bufHdr.x_off;
		int	y_off = bufctx->bufHdr.y_off;

		int stride_x = bufctx->bufHdr.stride_x;
		int stride_y = bufctx->bufHdr.stride_y;

		switch(datactx->transform){
			case 90:
	 			bufctx->bufHdr.x_off= bufctx->bufHdr.stride_y - datactx->height - y_off ;
 		   		bufctx->bufHdr.y_off= x_off ;
				bufctx->bufHdr.stride_x = stride_y;
				bufctx->bufHdr.stride_y = stride_x;
				break;
 
			case 270:
		 		bufctx->bufHdr.x_off= y_off ;
 				bufctx->bufHdr.y_off= bufctx->bufHdr.stride_x - datactx->width - x_off ;
				bufctx->bufHdr.stride_x = stride_y;
				bufctx->bufHdr.stride_y = stride_x;
				break;

			case 0:
			case 180:
				break;

			default:
				LOGE("%s, INVLAID rotate degree %d, only support 0, 90, 180 and 270 degree rotation",__FUNCTION__,datactx->transform);
                return -EINVAL;
		}

        TRACE("%s datactx->OvlySurface.viewPortInfo.srcWidth =%d, bufctx->bufHdr.stride_x =%d, bufctx->bufHdr.x_off=%d \n", 
 	   	__FUNCTION__, datactx->OvlySurface.viewPortInfo.srcWidth, bufctx->bufHdr.stride_x, bufctx->bufHdr.x_off );
 
        switch (datactx->OvlySurface.videoMode) {
			case FB_VMODE_YUV420PLANAR:
#ifdef _PXA168
                 // Use Pitch info to support ROI display
                 if(datactx->OvlySurface.viewPortInfo.srcWidth <= bufctx->bufHdr.stride_x) {
                     datactx->OvlySurface.viewPortInfo.yPitch = bufctx->bufHdr.stride_x;
                     datactx->OvlySurface.viewPortInfo.uPitch = bufctx->bufHdr.stride_x/2;
                     datactx->OvlySurface.viewPortInfo.vPitch = bufctx->bufHdr.stride_x/2;
                     TRACE("%s before display", __FUNCTION__);
                     TRACE("%s ViewPortInfo.yPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.yPitch);
                     TRACE("%s ViewPortInfo.uPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.uPitch);
                     TRACE("%s ViewPortInfo.vPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.vPitch);
                 }       
#else 
 	           LOGE("Invalid format setting, only support UYVY, RGB565 and RGBA888 on PXA955 platform!");
#endif
                 break;
 
            case FB_VMODE_YUV422PACKED:
            case FB_VMODE_RGB565:
#ifdef _PXA168
                            // Use Pitch info to support ROI display
                if(datactx->OvlySurface.viewPortInfo.srcWidth <= bufctx->bufHdr.stride_x) {
                    datactx->OvlySurface.viewPortInfo.yPitch =  bufctx->bufHdr.stride_x *2;
                    datactx->OvlySurface.viewPortInfo.uPitch = 0;
                    datactx->OvlySurface.viewPortInfo.vPitch = 0;
                    TRACE("%s before display", __FUNCTION__);
                    TRACE("%s ViewPortInfo.yPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.yPitch);
                    TRACE("%s ViewPortInfo.uPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.uPitch);
                    TRACE("%s ViewPortInfo.vPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.vPitch);
                }       
#else
                 /*PXA950*/
                 // Use Pitch info to support ROI display
                 if(datactx->OvlySurface.viewPortInfo.srcWidth <= bufctx->bufHdr.stride_x) {
                        datactx->OvlySurface.viewPortInfo.ycPitch = bufctx->bufHdr.stride_x *2;
                        datactx->OvlySurface.viewPortInfo.uvPitch = 0;
                        TRACE("%s before display", __FUNCTION__);
                        TRACE("%s ViewPortInfo.ycPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.ycPitch);
                        TRACE("%s ViewPortInfo.uvPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.uvPitch);
                 }       
#endif
 	           break;

            case FB_VMODE_RGBA888:
#ifdef _PXA168
                            // Use Pitch info to support ROI display
                if(datactx->OvlySurface.viewPortInfo.srcWidth <= bufctx->bufHdr.stride_x) {
                    datactx->OvlySurface.viewPortInfo.yPitch =  bufctx->bufHdr.stride_x *4;
                    datactx->OvlySurface.viewPortInfo.uPitch = 0;
                    datactx->OvlySurface.viewPortInfo.vPitch = 0;
                    TRACE("%s before display", __FUNCTION__);
                    TRACE("%s ViewPortInfo.yPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.yPitch);
                    TRACE("%s ViewPortInfo.uPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.uPitch);
                    TRACE("%s ViewPortInfo.vPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.vPitch);
                }       
#else
                 /*PXA950*/
                 // Use Pitch info to support ROI display
                 if(datactx->OvlySurface.viewPortInfo.srcWidth <= bufctx->bufHdr.stride_x) {
                        datactx->OvlySurface.viewPortInfo.ycPitch = bufctx->bufHdr.stride_x *4;
                        datactx->OvlySurface.viewPortInfo.uvPitch = 0;
                        TRACE("%s before display", __FUNCTION__);
                        TRACE("%s ViewPortInfo.ycPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.ycPitch);
                        TRACE("%s ViewPortInfo.uvPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.uvPitch);
                 }       
#endif
 	           break;

    	    default:
        	    LOGE("%s: Invalid format %d! Only support format: YUV420P, YUV422PACKED, RGB565 and RGBA888.)", __FUNCTION__,datactx->OvlySurface.videoMode);
            	return -EINVAL;
        }

	return 0;
}

static int display_buffer_size_process(struct overlay_data_context_t *datactx,
                                struct overlay_buffer_context_t *bufctx){
	/* Set the LCD display src size, which should be the same as transform dst size, based on rotate degree setting  */
	switch (datactx->transform) {
        case 90:
        case 270:
        	//rotate 90 or 270 degree
			datactx->OvlySurface.viewPortInfo.srcWidth = datactx->height;
			datactx->OvlySurface.viewPortInfo.srcHeight = datactx->width;
			break;
 
        case 180:
        case 0:
            //rotate 0 or 180 degree
			datactx->OvlySurface.viewPortInfo.srcWidth = datactx->width;
			datactx->OvlySurface.viewPortInfo.srcHeight = datactx->height;
 			break;
 
        default:
			LOGE("Invalid rotate degree: %d. Only support 0, 90, 180 and 270 degree rotation", datactx->transform);
 			return -EINVAL; 
    }
	
	return 0;
}


static int _overlay_convert_rotate_resize_process(struct overlay_data_context_t *datactx,
                                struct overlay_buffer_context_t *bufctx)
{
	TRACE("%s e", __FUNCTION__);

    char video_path[PROPERTY_VALUE_MAX];
    property_get(PROP_VIDEO_PATH, video_path, "normal");
    if (strcasecmp(video_path, "hdmi") == 0 ) return 0;

	char value[PROPERTY_VALUE_MAX];
	property_get(PROP_OVERLAY_PATH, value, "gc");

	void* tmpvaddr=bufctx->bufHdr.vaddr;
	void* tmppaddr=bufctx->bufHdr.paddr;

	//do the color convert firstly if necessary...
	if(datactx->colorspace_convert){
		//flush the cache to keep cache coherence before color space conversion 
		phy_cont_flush_cache(bufctx->bufHdr.vaddr, PHY_CONT_MEM_FLUSH_TO_DEVICE);

		//do colorspace convert in overlay before rotate and resize.
		if(_ipp_colorspace_convert_process(datactx, bufctx)){
			LOGE("%s error in ipp colorspace covert process", __FUNCTION__ );
			return -EINVAL;
		}
	}
	
	//flush the cache to keep cache coherence in case it has been updated. 
	phy_cont_flush_cache(bufctx->bufHdr.vaddr, PHY_CONT_MEM_FLUSH_TO_DEVICE);

	if ( strcmp(value, "gc") == 0){
		gcu_yuv_to_rgb_rotate( datactx, bufctx);
		/* Free buffer to the owner since the transform process is finished  */
		if(bufctx->bufHdr.free){
			bufctx->bufHdr.free(bufctx->bufHdr.user);
			bufctx->bufHdr.free = NULL;
		}
	}
	else if ( strcmp(value, "ipp") == 0){

		if(datactx->transform){
			ipp_yuv_rotate( datactx, bufctx);
			/* Free buffer to the owner since the transform process is finished  */
			if(bufctx->bufHdr.free){
				bufctx->bufHdr.free(bufctx->bufHdr.user);
				bufctx->bufHdr.free = NULL;
			}
		}

		display_buffer_size_process(datactx, bufctx);

		non_zero_start_point_process(datactx, bufctx);
	}
	else if ( strcmp(value, "ire") == 0){

		if(datactx->transform){
			_overlay_ire_process( datactx, bufctx);
			/* Free buffer to the owner since the transform process is finished  */
			if(bufctx->bufHdr.free){
				bufctx->bufHdr.free(bufctx->bufHdr.user);
				bufctx->bufHdr.free = NULL;
			}
		}

		display_buffer_size_process(datactx, bufctx);

		non_zero_start_point_process(datactx, bufctx);
	}
	else {
		LOGE("Invalid rotate path setting, only support IRE, GC and IPP rotation path!");
        	return -EINVAL;
	}

	//set the address back for free after LCD later.
	if(datactx->colorspace_convert){
		bufctx->bufHdr.vaddr= tmpvaddr;
		bufctx->bufHdr.paddr= tmppaddr;
	}

	TRACE("%s x", __FUNCTION__);
	return 0;
}

static int _overlay_lcd_display(struct overlay_data_context_t *datactx,
                                struct overlay_buffer_context_t *bufctx)
{
    TRACE("%s e", __FUNCTION__);

	char value[PROPERTY_VALUE_MAX];
	property_get(PROP_OVERLAY_PATH, value, "gc");

    char video_path[PROPERTY_VALUE_MAX];
    property_get(PROP_VIDEO_PATH, video_path, "normal");

    if (strcasecmp(video_path, "hdmi") == 0 ) {
        bufctx->pstart =  bufctx->bufHdr.paddr;
    }
    else{
        bufctx->pstart = ((datactx->transform > 0) ||  !strcmp(value, "gc")) ? bufctx->paddr_int_buf : bufctx->bufHdr.paddr;
    }

	int width  = datactx->width;
    int height = datactx->height;

    switch (datactx->OvlySurface.videoMode) {
    case FB_VMODE_YUV420PLANAR:
#ifdef _PXA168
		//Caculate the Y component start address based on the new start point offset
        datactx->OvlySurface.videoBufferAddr.startAddr[0] = (unsigned char*)bufctx->pstart
															+ bufctx->bufHdr.x_off + bufctx->bufHdr.y_off * bufctx->bufHdr.stride_x ;

		//Caculate the U component start address 
        datactx->OvlySurface.videoBufferAddr.startAddr[1] = (unsigned char*)bufctx->pstart  
															+  bufctx->bufHdr.stride_x * bufctx->bufHdr.stride_y 
															+ (bufctx->bufHdr.x_off/2 + bufctx->bufHdr.y_off * bufctx->bufHdr.stride_x/2) ;
		//Caculate the V component start address 
        datactx->OvlySurface.videoBufferAddr.startAddr[2] = (unsigned char*)bufctx->pstart 
															+  bufctx->bufHdr.stride_x * bufctx->bufHdr.stride_y 
															+ (bufctx->bufHdr.x_off/2 + bufctx->bufHdr.y_off * bufctx->bufHdr.stride_x/2) 
															+ (bufctx->bufHdr.stride_x * bufctx->bufHdr.stride_y >> 2);

		// bufctx->pstart must be the same as the address sent to LCD driver, otherwise, it can not be found in the free list and will be leaked.
		bufctx->pstart = datactx->OvlySurface.videoBufferAddr.startAddr[0];

		TRACE("%s: datactx->OvlySurface.videoBufferAddr.startAddr[0] = %p, bufctx->pstart =%p  ",__FUNCTION__ ,datactx->OvlySurface.videoBufferAddr.startAddr[0], bufctx->pstart);
		break;

#else 
		LOGE("Invalid format setting, only support UYVY, RGB565 and RGBA888 on PXA955 platform!");
#endif
		break;

	case FB_VMODE_YUV422PACKED:
#ifndef _PXA168 
        bufctx->pstart = (unsigned char*)bufctx->pstart + (bufctx->bufHdr.x_off + bufctx->bufHdr.y_off * bufctx->bufHdr.stride_x)*2;
        datactx->OvlySurface.videoBufferAddr.startAddr = (unsigned char*)bufctx->pstart ;
	 	break;
#endif

	case FB_VMODE_RGB565:
	case FB_VMODE_RGBA888:
#ifdef _PXA168
 	/*PXA910*/
        datactx->OvlySurface.videoBufferAddr.startAddr[0] = (unsigned char*)bufctx->pstart;
#else
 	/*PXA950*/
        datactx->OvlySurface.videoBufferAddr.startAddr = (unsigned char*)bufctx->pstart ;
#endif
	 	break;
    default:
        LOGE("%s: ERROR: only support YUV420P,YUV422PACKED,RGB565 and RGBA888 fromat %x)", __FUNCTION__,datactx->OvlySurface.videoMode);
        return -EINVAL;
    }

    datactx->OvlySurface.videoBufferAddr.frameID    = 0;
    datactx->OvlySurface.videoBufferAddr.inputData  = 0;	//set as NULL when buf is allocated in user space
    datactx->OvlySurface.videoBufferAddr.length	   = bufctx->bufHdr.len;

	TRACE("%s: frame number is %d, format(%x), %dx%d -> %dx%d, (%d, %d)",
         __FUNCTION__, flip_count++ , datactx->OvlySurface.videoMode,
        datactx->OvlySurface.viewPortInfo.srcWidth, datactx->OvlySurface.viewPortInfo.srcHeight,
        datactx->OvlySurface.viewPortInfo.zoomXSize, datactx->OvlySurface.viewPortInfo.zoomYSize,
        datactx->OvlySurface.viewPortOffset.xOffset, datactx->OvlySurface.viewPortOffset.yOffset);
#ifdef _PXA168
	// Use Pitch info to support ROI display
	TRACE("%s ViewPortInfo.yPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.yPitch);
	TRACE("%s ViewPortInfo.uPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.uPitch);
	TRACE("%s ViewPortInfo.vPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.vPitch);
#else
 	/*PXA950*/
	// Use Pitch info to support ROI display
	TRACE("%s ViewPortInfo.ycPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.ycPitch);
	TRACE("%s ViewPortInfo.uvPitch = %d\n", __FUNCTION__, datactx->OvlySurface.viewPortInfo.uvPitch);
#endif

    //TRACE("%s: call FB_IOCTL_FLIP_VID_BUFFER, startAddr = %p\n", __FUNCTION__, bufctx->pstart);
    if (ioctl(datactx->fd_ovly, FB_IOCTL_FLIP_VID_BUFFER, &datactx->OvlySurface))
    {
        LOGE("%s: ERROR: FB_IOCTL_FLIP_VID_BUFFER(%s)", __FUNCTION__, strerror(errno));
        return -EINVAL;
    }

    TRACE("%s x", __FUNCTION__);
    return 0;
}

static int _overlay_data_setupContext(struct overlay_data_context_t *datactx,
                                      const struct handle_t *obj)
{
    TRACE("%s e", __FUNCTION__);
    int i, j;
    int bufsize;

    /* sync to control module paramaters */
    datactx->fd_ovly    = obj->fd_ovly;
    datactx->format     = obj->format;
    datactx->transform  = obj->transform;
    datactx->resize  = obj->resize;
	
    datactx->cur_ctx = NULL;
    datactx->last_ctx = NULL;
    datactx->flag = 0;

    datactx->width  = obj->width;
    datactx->height = obj->height;

    /* get the baselay width and height */
    int fd;
    fd = open(BASELAY_DEVICE, O_RDWR);
    if (fd < 0) {
        LOGE("%s: ERROR: open baselay device(%s)", __FUNCTION__, strerror(errno));
        return NULL;
    }

    struct fb_var_screeninfo var;
    int ret = ioctl(fd, FBIOGET_VSCREENINFO, &var);
    close(fd);
    if (ret < 0) {
        LOGE("%s: ERROR: FBIOGET_VSCREENINFO(%s)", __FUNCTION__, strerror(errno));
        return NULL;
    }

	datactx->baselay_width  = var.xres;
	datactx->baselay_height = var.yres;

    if(obj->b_finput420sp)
	    datactx->colorspace_convert = 1; 
    else
	    datactx->colorspace_convert = 0; 
	
    for (i = 0; i < OVERLAY_QUEUE_CAPACITY; i++) {
	datactx->free_buf_ctx_array[i] = NULL;
	datactx->used_buf_ctx_array[i] = NULL;
    }

    /* allocate buffer context pool and queue them all to buffer context queue */
    datactx->buf_ctx_pool = (overlay_buffer_context_t *)
                      malloc(OVERLAY_QUEUE_CAPACITY * sizeof(overlay_buffer_context_t));
    memset(datactx->buf_ctx_pool, 0, OVERLAY_QUEUE_CAPACITY * sizeof(overlay_buffer_context_t));

	//Use RGB565 pixel number as default to allocate the dst buffer in overlay.
	int bytes_per_pixel = 2;;

	//When overlay 32bpp is enabled, use RGBA888 pixel number
	char value[PROPERTY_VALUE_MAX];
	property_get(PROP_OVERLAY_32BPP, value, "0");
	if ( strcmp(value, "1") == 0){
		bytes_per_pixel = 4;
	}

	if( ALIGN16(datactx->width) * ALIGN16(datactx->height) > datactx->baselay_width * datactx->baselay_height )
    	bufsize = ALIGN16(datactx->width) * ALIGN16(datactx->height) * bytes_per_pixel;
	else
    	bufsize = datactx->baselay_width * datactx->baselay_height * bytes_per_pixel;

    TRACE("intermediate buffer: format %d, %dx%d, size %d, width and height are both aligned to 16 pixels",
         datactx->format, datactx->width, datactx->height, bufsize);

    datactx->phycontHeap = (unsigned char *)phy_cont_malloc( (OVERLAY_QUEUE_CAPACITY + datactx->colorspace_convert)* bufsize, PHY_CONT_MEM_ATTR_DEFAULT);
    if (!datactx->phycontHeap) {
        LOGE("%s: failed to allocate physical continuous memory", __FUNCTION__);
        return -EINVAL;
    }

    for (i = 0; i < OVERLAY_QUEUE_CAPACITY ; i++) {
        /* get virtual address of IRE dest buffer */
	datactx->buf_ctx_pool[i].index         = i;
	datactx->buf_ctx_pool[i].vaddr_int_buf = (void *)(datactx->phycontHeap + i * bufsize);
	datactx->buf_ctx_pool[i].paddr_int_buf = (void *)phy_cont_getpa(datactx->buf_ctx_pool[i].vaddr_int_buf);
	datactx->buf_ctx_pool[i].pstart        = NULL;
	_EnArray(datactx->free_buf_ctx_array, &(datactx->buf_ctx_pool[i]), OVERLAY_QUEUE_CAPACITY);
	TRACE("intbuf[%d]: paddr = %p, vaddr = %p",
              i, datactx->buf_ctx_pool[i].paddr_int_buf, datactx->buf_ctx_pool[i].vaddr_int_buf);
    }
    if(datactx->colorspace_convert){
         datactx->colorconvert_buf_ctx.index=0;
         datactx->colorconvert_buf_ctx.bufHdr.vaddr=(void *)(datactx->phycontHeap + i * bufsize);
         datactx->colorconvert_buf_ctx.bufHdr.paddr=(void *)phy_cont_getpa(datactx->colorconvert_buf_ctx.bufHdr.vaddr);
         datactx->colorconvert_buf_ctx.pstart=NULL;
    }else{
         datactx->colorconvert_buf_ctx.index=0;
         datactx->colorconvert_buf_ctx.bufHdr.vaddr=NULL;
         datactx->colorconvert_buf_ctx.bufHdr.paddr=NULL;
         datactx->colorconvert_buf_ctx.pstart=NULL;
    }
	
    property_get(PROP_OVERLAY_PATH, value, "gc");

    if ( strcmp(value, "gc") == 0)
		gcu_init(datactx);
    else if ( strcmp(value, "ire") == 0)
		_overlay_ire_open(datactx);

    TRACE("%s x", __FUNCTION__);
    return 0;
}

static void _overlay_data_releaseContext(struct overlay_data_context_t *datactx)
{
    TRACE("%s e", __FUNCTION__);
    int i;
    overlay_buffer_context_t *bufctx;

    /*return buffer*/
    for (i = 0; i < OVERLAY_QUEUE_CAPACITY; i++) {
	bufctx = datactx->used_buf_ctx_array[i];
	if (bufctx) {
	    datactx->used_buf_ctx_array[i] = NULL;
		if(bufctx->bufHdr.free){
			bufctx->bufHdr.free(bufctx->bufHdr.user);
			bufctx->bufHdr.free = NULL;
		}
	}
    }

    /* clean up data context resources */
    if (datactx->buf_ctx_pool) {
        free(datactx->buf_ctx_pool);
        datactx->buf_ctx_pool = 0;
    }

    if(datactx->phycontHeap) {
        phy_cont_free(datactx->phycontHeap);
	datactx->phycontHeap = 0;
    }

    char value[PROPERTY_VALUE_MAX];
    property_get(PROP_OVERLAY_PATH, value, "gc");

	if ( strcmp(value, "gc") == 0)
		gcu_exit(datactx);
	else
	if ( strcmp(value, "ire") == 0)
		_overlay_ire_close(datactx);

    TRACE("%s x", __FUNCTION__);
}


// ****************************************************************************
// Control module
// ****************************************************************************

static int overlay_get(struct overlay_control_device_t *dev, int name) {
    TRACE("%s e", __FUNCTION__);
    int result = -1;
    switch (name) {
        case OVERLAY_MINIFICATION_LIMIT:
            result = 0; // 0 = no limit
            break;
        case OVERLAY_MAGNIFICATION_LIMIT:
            result = 0; // 0 = no limit
            break;
        case OVERLAY_SCALING_FRAC_BITS:
            result = 0; // 0 = infinite
            break;
        case OVERLAY_ROTATION_STEP_DEG:
            result = 90; // 90 rotation steps (for instance)
            break;
        case OVERLAY_HORIZONTAL_ALIGNMENT:
            result = 1; // 1-pixel alignment
            break;
        case OVERLAY_VERTICAL_ALIGNMENT:
            result = 1; // 1-pixel alignment
            break;
        case OVERLAY_WIDTH_ALIGNMENT:
            result = 1; // 1-pixel alignment
            break;
        case OVERLAY_HEIGHT_ALIGNMENT:
            result = 1; // 1-pixel alignment
            break;
    }
    TRACE("%s x", __FUNCTION__);
    return result;
}
 
static overlay_t* overlay_createOverlay(struct overlay_control_device_t *dev,
                                        uint32_t w, uint32_t h, int32_t format)
{
    TRACE("%s e", __FUNCTION__);
	overlay_t * overlay = NULL;
    /* check the input params, reject if not supported or invalid */
    FBVideoMode format_ovly;
	LOGD("%s: format = %d, w = %d, h =%d\n", __FUNCTION__, format, w, h);
    switch (format) {
        case OVERLAY_FORMAT_DEFAULT:
            format_ovly = FB_VMODE_YUV420PLANAR;
            LOGD("%s: default pixel format is YUV420Planar", __FUNCTION__);
            break;
        case OVERLAY_FORMAT_YCbCr_420_P:
//#if PLATFORM_SDK_VERSION <= 8   //GingerBread(9) has removed this macro
        case OVERLAY_FORMAT_YCbCr_420_SP:
//#endif
            format_ovly = FB_VMODE_YUV420PLANAR;
            LOGD("%s: pixel format is YUV420Planar", __FUNCTION__);
            break;
        case OVERLAY_FORMAT_YCbYCr_422_I:
            format_ovly = FB_VMODE_YUV422PACKED;
            LOGD("%s: pixel format is YUV422PACKED", __FUNCTION__);
            break;
         default:
            LOGE("%s: support YUV420Planar and YUV422PACKED as input, but the format is %x\n", __FUNCTION__, format);
            return NULL;
    }


    /* configure overlay device */
    int fd;
    fd = open(BASELAY_DEVICE, O_RDWR);
    if (fd < 0) {
        LOGE("%s: ERROR: open baselay device(%s)", __FUNCTION__, strerror(errno));
        return NULL;
    }

    struct fb_var_screeninfo var;
    int ret = ioctl(fd, FBIOGET_VSCREENINFO, &var);
    close(fd);
    if (ret < 0) {
        LOGE("%s: ERROR: FBIOGET_VSCREENINFO(%s)", __FUNCTION__, strerror(errno));
        return NULL;
    }
    int baselay_width  = var.xres;
    int baselay_height = var.yres;


    TRACE("%s: baselay_width = %d baselay_height = %d\n", __FUNCTION__, var.xres, var.yres);

    char video_path[PROPERTY_VALUE_MAX];
    property_get(PROP_VIDEO_PATH, video_path, "normal");

    if (strcasecmp(video_path, "hdmi") == 0 ) {
        fd = open(OVERLAY_HDMI_DEVICE, O_RDWR);

        if (fd < 0) {
            LOGE("%s: ERROR: open overlay device(%s)", __FUNCTION__, strerror(errno));
            return NULL;
        }
        property_set(PROP_OVERLAY_ROTATE, "0");
        TRACE("%s: open hdmi path, rotate is disabled", __FUNCTION__);
    } else {
        fd = open(OVERLAY2_DEVICE, O_RDWR);
        if (fd < 0) {
            LOGE("%s: ERROR: open overlay device(%s)", __FUNCTION__, strerror(errno));
            return NULL;
        }
   }
    /*
     * turn off overlay DMA transfer and turn on later
     * when first frame is ready to be displayed
     */
    int on = 0;
    if (ioctl(fd, FB_IOCTL_SWITCH_VID_OVLY, &on)) {
        LOGE("%s: ERROR: FB_IOCTL_SWITCH_VID_OVLY(on)(%s)", __FUNCTION__, strerror(errno));
    }

    struct _sColorKeyNAlpha colorkey;
#ifdef _PXA168
    colorkey.mode              = FB_ENABLE_RGB_COLORKEY_MODE;
    colorkey.alphapath         = FB_GRA_PATH_ALPHA;
    colorkey.config            = 0x20;//0x20;
    colorkey.Y_ColorAlpha      = 0x4;
    colorkey.U_ColorAlpha      = 0x4;
    colorkey.V_ColorAlpha      = 0x4;
    if (ioctl(fd, FB_IOCTL_SET_COLORKEYnALPHA, &colorkey)) {
        LOGE("%s: ERROR: FB_IOCTL_SET_COLORKEYnALPHA(%s)", __FUNCTION__, strerror(errno));
        close(fd);
        return NULL;
    }
#else
	/*PXA950*/
    //colorkey.win_alpha_en      = 0x0;
    //colorkey.alpha_method      = 0x0;
    //colorkey.default_alpha_val = 0x4;
    //colorkey.color_match       = 0x0;
#endif



    static struct _sViewPortInfo ViewPortInfo;
    static struct _sViewPortOffset ViewPortOffset;

    /* get rotate info from proterty */
    char value[PROPERTY_VALUE_MAX];
    property_get(PROP_OVERLAY_ROTATE, value, "90");
    int transform = atoi(value);
	
	/* get resize info from proterty */
    property_get(PROP_OVERLAY_RESIZE, value, "0");
    int resize = atoi(value);

    ViewPortInfo.srcWidth  = w;
    ViewPortInfo.srcHeight = h;

	ViewPortInfo.zoomXSize = ViewPortInfo.srcWidth ;
	ViewPortInfo.zoomYSize = ViewPortInfo.srcHeight;
	ViewPortInfo.rotation  = 0;
	ViewPortInfo.yuv_format = 0;

#ifdef _PXA168
	ViewPortInfo.yPitch    = ViewPortInfo.srcWidth * 2;
	ViewPortInfo.uPitch    = 0;
	ViewPortInfo.vPitch    = 0;
#else
	ViewPortInfo.ycPitch    = ViewPortInfo.srcWidth * 2;
	ViewPortInfo.uvPitch    = 0;
#endif

    ViewPortOffset.xOffset = 0;
    ViewPortOffset.yOffset = 0;

	LOGD("%s: Video Source: format(%x), %dx%d -> %dx%d, (%d, %d)",
         __FUNCTION__, format_ovly,
         ViewPortInfo.srcWidth, ViewPortInfo.srcHeight,
         ViewPortInfo.zoomXSize, ViewPortInfo.zoomYSize,
         ViewPortOffset.xOffset, ViewPortOffset.yOffset);

    struct _sOvlySurface OvlySurface;
	memset(&OvlySurface, 0, sizeof(_sOvlySurface));

	OvlySurface.videoMode = format_ovly;
	OvlySurface.viewPortInfo = ViewPortInfo;
	OvlySurface.viewPortOffset = ViewPortOffset;

	if (ioctl(fd, FB_IOCTL_SET_VIEWPORT_INFO, &OvlySurface.viewPortInfo)) {
        LOGE("%s: ERROR: FB_IOCTL_SET_VIEWPORT_INFO(%s)", __FUNCTION__, strerror(errno));
        close(fd);
        return NULL;
    }


    if (ioctl(fd, FB_IOCTL_SET_VID_OFFSET, &OvlySurface.viewPortOffset)) {
        LOGE("%s : ERROR: FB_IOCTL_SET_VIEWPORT_INFO(%s)", __FUNCTION__, strerror(errno));
        close(fd);
        return NULL;
    }

    /* always set VideoMode after ViewPortInfo, or driver will overwrite it */
    if (ioctl(fd, FB_IOCTL_SET_VIDEO_MODE, &OvlySurface.videoMode )) {
        LOGE("%s: ERROR: FB_IOCTL_SET_VIDEO_MODE(%s) %x\n", __FUNCTION__, strerror(errno), format_ovly);
        close(fd);
        return NULL;
    }

    /* Create overlay object. Talk to the h/w here and adjust to what it can
     * do. the overlay_t returned can  be a C++ object, subclassing overlay_t
     * if needed.
     *
     * we probably want to keep a list of the overlay_t created so they can
     * all be cleaned up in overlay_close().
     */

    if (ioctl(fd, FB_IOCTL_GET_VIDEO_MODE, &OvlySurface.videoMode)) {
        LOGE("%s: ERROR: FB_IOCTL_GET_VIDEO_MODE(%s)", __FUNCTION__, strerror(errno));
        return NULL;
    }
    TRACE("%s: videoMode = %d\n", __FUNCTION__, OvlySurface.videoMode);


    if (ioctl(fd, FB_IOCTL_GET_VIEWPORT_INFO, &OvlySurface.viewPortInfo)) {
        LOGE("%s: ERROR: FB_IOCTL_GET_VIEWPORT_INFO(%s)", __FUNCTION__, strerror(errno));
        return NULL;
    }
    TRACE("%s: zoomXSize = %d zoomYSize = %d\n", __FUNCTION__, OvlySurface.viewPortInfo.zoomXSize, OvlySurface.viewPortInfo.zoomYSize);

    if (ioctl(fd, FB_IOCTL_GET_VID_OFFSET, &OvlySurface.viewPortOffset)) {
        LOGE("%s: ERROR: FB_IOCTL_GET_VID_OFFSET(%s)", __FUNCTION__, strerror(errno));
        return NULL;
    }
    TRACE("%s: LCD display: format(%x), %dx%d -> %dx%d, (%d, %d)",
         __FUNCTION__, OvlySurface.videoMode,
         OvlySurface.viewPortInfo.srcWidth, OvlySurface.viewPortInfo.srcHeight,
         OvlySurface.viewPortInfo.zoomXSize, OvlySurface.viewPortInfo.zoomYSize,
         OvlySurface.viewPortOffset.xOffset, OvlySurface.viewPortOffset.yOffset);


    TRACE("%s x", __FUNCTION__);
    return new overlay_object(fd, format_ovly,
                              ViewPortInfo.srcWidth, ViewPortInfo.srcHeight,
                              ViewPortInfo.zoomXSize, ViewPortInfo.zoomYSize,
                              ViewPortOffset.xOffset, ViewPortOffset.yOffset,
                              transform, resize, OvlySurface, true,baselay_width, baselay_height, (format==OVERLAY_FORMAT_YCbCr_420_SP)?true:false);

}

static void overlay_destroyOverlay(struct overlay_control_device_t *dev,
                                   overlay_t* overlay)
{
    TRACE("%s e", __FUNCTION__);
    /* free resources associated with this overlay_t */
    overlay_object *obj = static_cast<overlay_object *>(overlay);

    /*close fd of overlay*/
    /*
    if (obj->mHandle.fd_ovly) {
	close(obj->mHandle.fd_ovly);
    }
*/

	/*switch off overlay*/
    if (obj->mHandle.fd_ovly > 0) {
    int ovlyOn = 0;
        if (ioctl(obj->mHandle.fd_ovly, FB_IOCTL_SWITCH_VID_OVLY, &ovlyOn)) {
            LOGE("%s: ERROR: FB_IOCTL_SWITCH_VID_OVLY(off)(%s)", __FUNCTION__, strerror(errno));
        }
	close(obj->mHandle.fd_ovly);
    }

	LOGD("%s: destroy overlay object", __FUNCTION__);
    delete obj;

    TRACE("%s x", __FUNCTION__);
}

static int overlay_setPosition(struct overlay_control_device_t *dev,
                               overlay_t* overlay,
                               int x, int y, uint32_t w, uint32_t h)
{
    TRACE("%s e", __FUNCTION__);
    /* set this overlay's position (talk to the h/w) */
    overlay_object *obj = static_cast<overlay_object *>(overlay);

    TRACE("%s: x = %d y = %d w = %d h = %d\n", __FUNCTION__, x, y, w, h);

    if( x < 0 || (uint32_t)x >= obj->baselay_width
       || y < 0 || (uint32_t)y >= obj->baselay_height
       || x + w > obj->baselay_width
       || y + h > obj->baselay_height ) {
       LOGE("%s: invalid position %dx%d, (%d, %d), baselay is %dx%d",
            __FUNCTION__, w, h, x, y, obj->baselay_width, obj->baselay_height);
	   obj->mHandle.commit_flag = false;
	   return -EINVAL;
    }
	else{
		obj->mHandle.commit_flag = true;
	}
		
	
	struct fb_fix_screeninfo fix;
    int err;

    char video_path[PROPERTY_VALUE_MAX];
    property_get(PROP_VIDEO_PATH, video_path, "normal");

	char ov_path[PROPERTY_VALUE_MAX];
    property_get(PROP_OVERLAY_PATH, ov_path, "gc");

	char resize_disable[PROPERTY_VALUE_MAX];
	property_get(PROP_GCU_RESIZE_DISABLE, resize_disable, "0");
	int resize_disabled = atoi(resize_disable);
			
    char value[PROPERTY_VALUE_MAX];
	property_get(PROP_OVERLAY_PATH, value, "gc");

	/* In following cases, resize is disabled in overlay hal:
	 * 1. overlay.gcu.resize.disable set to 1
	 * 2. rotate engine is not GCU
	 * 3. output to HDMI
	 * 4. zoom size is not smaller than video size
	 */
	if( resize_disabled || (strcmp(value, "gc") != 0) || (strcasecmp(video_path, "hdmi") == 0)  ){
		LOGD("Resize is disabled in overlay hal.");
    	// user_data[1]: store the resize setting information
		obj->mHandle.surface.user_data[1] = false;
	}else {
	
			/* Set the LCD display src size, which should be the same as transform dst size. */
			LOGD("%s Resize the video to be (%d)x(%d) in overlay hal", __FUNCTION__,w,h);

			// user_data[1]: store the resize setting information
			obj->mHandle.surface.user_data[1] = true;
	}



	memset(&fix,0,sizeof(fix));

    if (strcasecmp(video_path, "hdmi") == 0){
        obj->mHandle.zoom_width  = obj->mHandle.surface.viewPortInfo.srcWidth;
        obj->mHandle.zoom_height = obj->mHandle.surface.viewPortInfo.srcHeight;
	
        obj->mHandle.surface.viewPortInfo.zoomXSize = obj->mHandle.surface.viewPortInfo.srcWidth;
        obj->mHandle.surface.viewPortInfo.zoomYSize = obj->mHandle.surface.viewPortInfo.srcHeight;

        obj->mHandle.surface.viewPortOffset.xOffset = 0 ;
        obj->mHandle.surface.viewPortOffset.yOffset = 0 ;
        TRACE("%s, resize in LCD is disabled", __FUNCTION__);
    } else {
		obj->mHandle.offset_x    = x;
        obj->mHandle.offset_y    = y;
        obj->mHandle.zoom_width  = w;
        obj->mHandle.zoom_height = h;
    
		/* Set the LCD display dst size*/
        obj->mHandle.surface.viewPortInfo.zoomXSize = w;
        obj->mHandle.surface.viewPortInfo.zoomYSize = h;
    
		obj->mHandle.surface.viewPortOffset.xOffset = x;
        obj->mHandle.surface.viewPortOffset.yOffset = y;
	}

    if (ioctl(obj->mHandle.fd_ovly, FBIOGET_FSCREENINFO, &fix)) {
        LOGE("%s: ERROR: FBIOGET_FSCREENINFO(%s)", __FUNCTION__, strerror(errno));
        return -EINVAL;
    }

    TRACE("%s: %dx%d -> %dx%d, (%d, %d)", __FUNCTION__,
         obj->mHandle.surface.viewPortInfo.srcWidth, 
		 obj->mHandle.surface.viewPortInfo.srcHeight,
         obj->mHandle.surface.viewPortInfo.zoomXSize, 
		 obj->mHandle.surface.viewPortInfo.zoomYSize,
         obj->mHandle.surface.viewPortOffset.xOffset, 
		 obj->mHandle.surface.viewPortOffset.yOffset);
	
	TRACE("%s x", __FUNCTION__);
    return 0;
}

static int overlay_getPosition(struct overlay_control_device_t *dev,
                               overlay_t* overlay,
                               int* x, int* y, uint32_t* w, uint32_t* h)
{
    TRACE("%s e", __FUNCTION__);
    /* get this overlay's position */
    overlay_object *obj = static_cast<overlay_object *>(overlay);
    *x = obj->mHandle.offset_x;
    *y = obj->mHandle.offset_y;
    *w = obj->mHandle.zoom_width;
    *h = obj->mHandle.zoom_height;

    TRACE("%s: %dx%d, (%d, %d)", __FUNCTION__, *x, *y, *w, *h);

    TRACE("%s x", __FUNCTION__);
    return 0;
}
ROTATION_DEGREE get_rotate_degree(int overlay_rotate) {
    TRACE("%s e", __FUNCTION__);
    ROTATION_DEGREE rotate_degree = ROT_DEGREE_90;
	int rotate;

    char video_path[PROPERTY_VALUE_MAX];
    property_get(PROP_VIDEO_PATH, video_path, "normal");

    if (strcasecmp(video_path, "hdmi") == 0) {
        rotate_degree = ROT_DEGREE_0;
        return rotate_degree;
    }
    
	char rotate_sensor[PROPERTY_VALUE_MAX];
	property_get(PROP_ROTATE_SENSOR, rotate_sensor, "1");

	if(atoi(rotate_sensor)){

		TRACE("%s overlay_rotate = %d", __FUNCTION__, overlay_rotate);
        switch( overlay_rotate ) {
            case HAL_TRANSFORM_ROT_90:
                rotate_degree = ROT_DEGREE_90;
                break;
            case HAL_TRANSFORM_ROT_180:
                rotate_degree = ROT_DEGREE_180;
                break;
            case HAL_TRANSFORM_ROT_270:
                rotate_degree = ROT_DEGREE_270;
                break;
            default:
                rotate_degree = ROT_DEGREE_0;
                break;
        }
		TRACE(" %s overlay_rotate = %d, rotate_degree = %d", __FUNCTION__, overlay_rotate, rotate_degree);
	}
	else {
		char rotate_prop[PROPERTY_VALUE_MAX];
		property_get(PROP_OVERLAY_ROTATE, rotate_prop, "90");
		rotate = atoi(rotate_prop);

		TRACE("%s rotate = %d", __FUNCTION__, rotate);
        switch( rotate ) {
            case 0:
                rotate_degree = ROT_DEGREE_0 ;
                break;
            case 90:
                rotate_degree = ROT_DEGREE_90;
                break;
            case 180:
                rotate_degree = ROT_DEGREE_180;
                break;
            case 270:
                rotate_degree = ROT_DEGREE_270;
                break;
            default:
                break;
        }
		TRACE(" %s rotate = %d, rotate_degree = %d", __FUNCTION__, rotate, rotate_degree);
	}

	TRACE("%s x", __FUNCTION__);
    return rotate_degree;
}

int overlay_rotate_setting(overlay_t* overlay, int value){

	overlay_object *obj = static_cast<overlay_object *>(overlay);
    int ret = -EINVAL;
    const int rotate_deg[] = {
        0,
        90,
        180,
        270
    };

	ROTATION_DEGREE rotate = get_rotate_degree(value);
	
	// user_data[0]: store the rotate degree information
	obj->mHandle.surface.user_data[0] = rotate_deg[rotate];
	LOGD("%s: rotate %d degree \n", __FUNCTION__, obj->mHandle.surface.user_data[0]);

	return 0;
}

int overlay_switch_setting(overlay_t* overlay, int value){
       overlay_object *obj = static_cast<overlay_object *>(overlay);
		int ovlyOn;

		if(value){
			LOGD("%s: Switch on overlay\n", __FUNCTION__);
			ovlyOn = 1;	
		}
		else {
			LOGD("%s: Switch off overlay\n", __FUNCTION__);
			ovlyOn = 0;	
		}

		TRACE("%s: overlay switch setting is %d\n", __FUNCTION__, ovlyOn);
    	if (ioctl( obj->mHandle.fd_ovly, FB_IOCTL_SWITCH_VID_OVLY, &ovlyOn)) {
        LOGE("%s: ERROR: FB_IOCTL_SWITCH_VID_OVLY(on)(%s)", __FUNCTION__, strerror(errno));
               return -1;
       }
       return 0;
}

static int overlay_setParameter(struct overlay_control_device_t *dev,
                                overlay_t* overlay, int param, int value)
{
    TRACE("%s e", __FUNCTION__);
    int result = 0;
    /* set this overlay's parameter (talk to the h/w) */
    switch (param) {
        case OVERLAY_ROTATION_DEG:
            /* if only 90 rotations are supported, the call fails
             * for other values */
            break;
        case OVERLAY_DITHER:
            break;
        case OVERLAY_TRANSFORM:
			TRACE("%s OVERLAY_TRANSFORM_ value = %d", __FUNCTION__, value);
			overlay_rotate_setting(overlay, value);
            // see OVERLAY_TRANSFORM_*
           break;
       case OVERLAY_SWITCH:
       		TRACE("%s: overlay switch setting is %d\n", __FUNCTION__, value);
                       overlay_switch_setting(overlay, value);
            break;
	
		default:
			TRACE("%s default ", __FUNCTION__);
            result = -EINVAL;
            break;
    }
    TRACE("%s x", __FUNCTION__);
    return result;
}

static int overlay_stage(struct overlay_control_device_t *dev, overlay_t* overlay)
{
    TRACE("%s e", __FUNCTION__);
    TRACE("%s x", __FUNCTION__);
    return 0;
}

static int overlay_commit(struct overlay_control_device_t *dev, overlay_t* overlay)
{
    TRACE("%s e", __FUNCTION__);

	overlay_object *obj = static_cast<overlay_object *>(overlay);

    TRACE("%s obj->mHandle.commit_flag =%d", __FUNCTION__, obj->mHandle.commit_flag);

	if(!(obj->mHandle.commit_flag))return 0;
	
    TRACE("%s: before commit: %dx%d -> %dx%d, (%d, %d)", __FUNCTION__,
         obj->mHandle.surface.viewPortInfo.srcWidth, 
		 obj->mHandle.surface.viewPortInfo.srcHeight,
         obj->mHandle.surface.viewPortInfo.zoomXSize, 
		 obj->mHandle.surface.viewPortInfo.zoomYSize,
         obj->mHandle.surface.viewPortOffset.xOffset, 
		 obj->mHandle.surface.viewPortOffset.yOffset);

	if (ioctl(obj->mHandle.fd_ovly, FB_IOCTL_SET_SURFACE, &obj->mHandle.surface)) {
		LOGE("%s: ERROR: FB_IOCTL_SET_SURFACE failed (%s)", __FUNCTION__, strerror(errno));
		return -EINVAL;
	}

	struct _sOvlySurface OvlySurface;	

	if (ioctl(obj->mHandle.fd_ovly, FB_IOCTL_GET_SURFACE, &OvlySurface)) {
       	LOGE("%s: ERROR: FB_IOCTL_GET_SURFACE failed (%s)", __FUNCTION__, strerror(errno));
       	return -EINVAL;
    }

	TRACE("%s: format(%x), %dx%d -> %dx%d, (%d, %d)",
         __FUNCTION__, OvlySurface.videoMode,
         OvlySurface.viewPortInfo.srcWidth, OvlySurface.viewPortInfo.srcHeight,
         OvlySurface.viewPortInfo.zoomXSize, OvlySurface.viewPortInfo.zoomYSize,
         OvlySurface.viewPortOffset.xOffset, OvlySurface.viewPortOffset.yOffset);

	TRACE("%s x", __FUNCTION__);
    return 0;
}

static int overlay_control_close(struct hw_device_t *dev)
{
    struct overlay_control_context_t* ctx = (struct overlay_control_context_t*)dev;

    TRACE("%s e", __FUNCTION__);

    if (ctx) {
        /* free all resources associated with this device here
         * in particular the overlay_handle_t, outstanding overlay_t, etc...
         */
        TRACE("%s: close overlay control module(%p)", __FUNCTION__, ctx);
        free(ctx);
        ctx = 0;
    }
    TRACE("%s x", __FUNCTION__);
    return 0;
}

// ****************************************************************************
// Data module
// ****************************************************************************

static int overlay_initialize(struct overlay_data_device_t *dev,
                              overlay_handle_t handle)
{
    /*
     * overlay_handle_t should contain all the information to "inflate" this
     * overlay. Typically it'll have a file descriptor, informations about
     * how many buffers are there, etc...
     * It is also the place to mmap all buffers associated with this overlay
     * (see getBufferAddress).
     *
     * NOTE: this FUNCTIONtion doesn't take ownership of overlay_handle_t
     *
     */
    struct overlay_data_context_t *datactx = (struct overlay_data_context_t *)dev;
    const struct handle_t *obj = static_cast<const struct handle_t*>(handle);

    LOGD("%s: initialize overlay data module(%p)", __FUNCTION__, datactx);
    /*for debug*/
    //sleep(30);

    if (_overlay_data_setupContext(datactx, obj)) {
        LOGE("Failed to initialize overlay data module");
        _overlay_data_releaseContext(datactx);
        return -EINVAL;
    }

        return 0;
}

static int overlay_resizeInput(struct overlay_data_device_t *dev,
                               uint32_t w, uint32_t h)
{
    TRACE("%s e", __FUNCTION__);
    TRACE("%s x", __FUNCTION__);
    return -EINVAL;
}

static int overlay_setCrop(struct overlay_data_device_t *dev,
            uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    TRACE("%s e", __FUNCTION__);
    TRACE("%s x", __FUNCTION__);
    return -EINVAL;
}

static int overlay_getCrop(struct overlay_data_device_t *dev,
            uint32_t* x, uint32_t* y, uint32_t* w, uint32_t* h)
{
    TRACE("%s e", __FUNCTION__);
    TRACE("%s x", __FUNCTION__);
    return -EINVAL;
}

static int overlay_data_setParameter(struct overlay_data_device_t *dev,
            int param, int value)
{
    TRACE("%s e", __FUNCTION__);
    TRACE("%s x", __FUNCTION__);
    return -EINVAL;
}

#define DUMP_USED_BUF_CTX_ARRAY(bufctx) \
{\
	int i;\
	for (i = 0; i < OVERLAY_QUEUE_CAPACITY; i++) {\
		if (bufctx[i]) {\
			TRACE("%s: used buf ctx: index = %d address = %p\n", __FUNCTION__, bufctx[i]->index, bufctx[i]);\
		}\
	}\
}

#define DUMP_FREE_BUF_CTX_ARRAY(bufctx) \
{\
	int i;\
	for (i = 0; i < OVERLAY_QUEUE_CAPACITY; i++) {\
		if (bufctx[i]) {\
			TRACE("%s: free buf ctx: index = %d address = %p\n", __FUNCTION__, bufctx[i]->index, bufctx[i]);\
		}\
	}\
}


static int overlay_dequeueBuffer(struct overlay_data_device_t *dev,
                                 overlay_buffer_t* buf)
{
    /* blocks until a buffer is available and return an opaque structure
     * representing this buffer.
     */
    struct overlay_data_context_t *datactx = (struct overlay_data_context_t *)dev;
    int i, j;
    struct overlay_buffer_context_t *bufctx;

    TRACE("%s e", __FUNCTION__);
    //DUMP_QUEUE;

    /*
     * keep querying LCD driver until free buffers are avaliable.
     * time out after trying 10 times.
     */
    /* we has to use MAX_QUEUE_NUM here, or driver will destroy our stack */
#ifdef _PXA168
    void* freeBufAddr[MAX_QUEUE_NUM][3];
#else
    /*PAX950*/
    void* freeBufAddr[MAX_QUEUE_NUM][1];
#endif
    memset(freeBufAddr, 0, sizeof(freeBufAddr));


//    DUMP_USED_BUF_CTX_ARRAY(datactx->used_buf_ctx_array);
//    DUMP_FREE_BUF_CTX_ARRAY(datactx->free_buf_ctx_array);

getbuf:
    TRACE("Get free list from LCD driver:");
    for (i = 0; i < 10; i++) {
        if (ioctl(datactx->fd_ovly, FB_IOCTL_GET_FREELIST, freeBufAddr)) {
            LOGE("ERROR: FB_IOCTL_GET_FREELIST(%s). Try again...", strerror(errno));
        }
        else {
            break;
        }
    }

    if (i == 10) {
        LOGE("ERROR: FB_IOCTL_GET_FREELIST time out");
        return -EINVAL;
    }

    /*
     * match free buffers to overlay buffer contexts and
     * push back matched buffer contexts to buffer context queue.
     */
    void *addr;
    for (i = 0; i < MAX_QUEUE_NUM; i++) {
        addr = freeBufAddr[i][0];
	if (!addr) {
	    continue;
	}
        TRACE("%s: freeBufAddr[%d][0]: %p", __FUNCTION__, i, addr);
	for (j = 0; j < OVERLAY_QUEUE_CAPACITY; j++) {
	    bufctx = datactx->used_buf_ctx_array[j];
	    if (bufctx) {
		if (bufctx->pstart == addr) {
		    datactx->used_buf_ctx_array[j] = NULL;
			if(bufctx->bufHdr.free){
				bufctx->bufHdr.free(bufctx->bufHdr.user);
				bufctx->bufHdr.free = NULL;
			}

		    _EnArray(datactx->free_buf_ctx_array, bufctx, OVERLAY_QUEUE_CAPACITY);
		    break;
		}
	    }
	}
    }
    TRACE("%s: get and match free list Done", __FUNCTION__);

//    DUMP_USED_BUF_CTX_ARRAY(datactx->used_buf_ctx_array);
//    DUMP_FREE_BUF_CTX_ARRAY(datactx->free_buf_ctx_array);
    /* dequeue a buffer context and deliver to application*/
    _DeArray(datactx->free_buf_ctx_array, (struct overlay_buffer_context_t **)&bufctx, OVERLAY_QUEUE_CAPACITY);
    if (!bufctx) {
	LOGE("%s: free buffer array is null!\n", __FUNCTION__);
        usleep(1000);
        //sleep(1);
	goto getbuf;
    }

	datactx->cur_ctx = bufctx;

	memset(&(bufctx->bufHdr), 0, sizeof(bufctx->bufHdr));

	bufctx->bufHdr.stride_x = datactx->width;
	bufctx->bufHdr.stride_y = datactx->height;
	bufctx->bufHdr.x_off = 0;
	bufctx->bufHdr.y_off = 0;

    //TRACE("%s: bufCtx = %p\n", __FUNCTION__, bufctx);
    *buf = &(bufctx->bufHdr);

    TRACE("%s x", __FUNCTION__);
    return 0;
}


static int free_codec_buffer(struct overlay_data_context_t *datactx,
                                struct overlay_buffer_context_t *bufctx)
{
    TRACE("%s e", __FUNCTION__);

    /*
     * keep querying LCD driver until free buffers are avaliable.
     * time out after trying 10 times.
     */
    /* we has to use MAX_QUEUE_NUM here, or driver will destroy our stack */
#ifdef _PXA168
    void* freeBufAddr[MAX_QUEUE_NUM][3];
#else
    /*PAX950*/
    void* freeBufAddr[MAX_QUEUE_NUM][1];
#endif
	int i,j;
    memset(freeBufAddr, 0, sizeof(freeBufAddr));


//    DUMP_USED_BUF_CTX_ARRAY(datactx->used_buf_ctx_array);
//    DUMP_FREE_BUF_CTX_ARRAY(datactx->free_buf_ctx_array);

getbuf:
    TRACE("Get free list from LCD driver:");
    for (i = 0; i < 10; i++) {
        if (ioctl(datactx->fd_ovly, FB_IOCTL_GET_FREELIST, freeBufAddr)) {
            LOGE("ERROR: FB_IOCTL_GET_FREELIST(%s). Try again...", strerror(errno));
        }
        else {
            break;
        }
    }

    if (i == 10) {
        LOGE("ERROR: FB_IOCTL_GET_FREELIST time out");
        return -EINVAL;
    }

    /*
     * match free buffers to overlay buffer contexts and
     * push back matched buffer contexts to buffer context queue.
     */
    void *addr;
    for (i = 0; i < MAX_QUEUE_NUM; i++) {
        addr = freeBufAddr[i][0];
	if (!addr) {
	    continue;
	}
        TRACE("%s: freeBufAddr[%d][0]: %p", __FUNCTION__, i, addr);
	for (j = 0; j < OVERLAY_QUEUE_CAPACITY; j++) {
	    bufctx = datactx->used_buf_ctx_array[j];
	    if (bufctx) {
		if (bufctx->pstart == addr) {
		    datactx->used_buf_ctx_array[j] = NULL;
			if(bufctx->bufHdr.free){
				bufctx->bufHdr.free(bufctx->bufHdr.user);
				bufctx->bufHdr.free = NULL;
			}

		    _EnArray(datactx->free_buf_ctx_array, bufctx, OVERLAY_QUEUE_CAPACITY);
		    break;
		}
	    }
	}
    }
    TRACE("%s: get and match free list Done", __FUNCTION__);

    TRACE("%s x", __FUNCTION__);
	return 0;

}

static int overlay_queueBuffer(struct overlay_data_device_t *dev,
                               overlay_buffer_t buffer)
{

#ifdef QUEUE_TIME_MEASURE
	clock_gettime(CLOCK_REALTIME, &overlay_time[0]);
#endif

	/* Mark this buffer for posting and recycle or free overlay_buffer_t. */
    struct overlay_data_context_t *datactx  = (struct overlay_data_context_t *)dev;
    struct overlay_buffer_context_t *bufctx = (struct overlay_buffer_context_t *)buffer;
    int i = 0;

    TRACE("%s e", __FUNCTION__);

    if (bufctx->bufHdr.flag == 5) {  
          if ( ioctl(datactx->fd_ovly, FB_IOCTL_WAIT_VSYNC, NULL) ) {  
              LOGE("!!!! FB_IOCTL_WAIT_VSYNC wrong");  
          }  
         return 0;  
    }
    else if (bufctx->bufHdr.flag == 4) {  
        return free_codec_buffer(datactx, bufctx);
    }
    else if (!bufctx->bufHdr.paddr){
                LOGE("Invalid NULL physical buffer address is passed, drop this buffer");
                return 0;
    }

#ifdef QUEUE_TIME_MEASURE
	caculate_delta_time(overlay_time[0], overlay_time[1]);
#endif

	if (ioctl(datactx->fd_ovly, FB_IOCTL_GET_SURFACE, &datactx->OvlySurface)) {
        LOGE("%s: ERROR: FB_IOCTL_GET_SURFACE failed (%s)", __FUNCTION__, strerror(errno));
        return -EINVAL;
    }

	TRACE("%s:  format(%x), %dx%d -> %dx%d, (%d, %d)",
         __FUNCTION__, datactx->OvlySurface.videoMode,
         datactx->OvlySurface.viewPortInfo.srcWidth, datactx->OvlySurface.viewPortInfo.srcHeight,
         datactx->OvlySurface.viewPortInfo.zoomXSize, datactx->OvlySurface.viewPortInfo.zoomYSize,
         datactx->OvlySurface.viewPortOffset.xOffset, datactx->OvlySurface.viewPortOffset.yOffset);

	// user_data[0]: store the rotate degree information
	datactx->transform = datactx->OvlySurface.user_data[0];

	// user_data[1]: store the resize setting information
	datactx->resize =  datactx->OvlySurface.user_data[1];
	TRACE("%s: datactx->OvlySurface: rotate_degree = %d, resize_setting = %d", __FUNCTION__, datactx->OvlySurface.user_data[0] , datactx->OvlySurface.user_data[1]);

#ifdef QUEUE_TIME_MEASURE
	caculate_delta_time(overlay_time[1], overlay_time[2]);
#endif

	_overlay_convert_rotate_resize_process(datactx, bufctx);

#ifdef QUEUE_TIME_MEASURE
	caculate_delta_time(overlay_time[2], overlay_time[3]);
#endif
#ifdef QUEUE_TIME_MEASURE
	caculate_delta_time(overlay_time[3], overlay_time[4]);
#endif

	if( !datactx->OvlySurface.viewPortInfo.zoomXSize || !datactx->OvlySurface.viewPortInfo.zoomYSize )
	{
		LOGW("Invalid buffer surface zoom size setting: (%d)x(%d), drop it.\n",
				datactx->OvlySurface.viewPortInfo.zoomXSize, datactx->OvlySurface.viewPortInfo.zoomYSize);

		datactx->used_buf_ctx_array[i] = NULL;
		if(bufctx->bufHdr.free){
			bufctx->bufHdr.free(bufctx->bufHdr.user);
			bufctx->bufHdr.free = NULL;
		}

		_EnArray(datactx->free_buf_ctx_array, bufctx, OVERLAY_QUEUE_CAPACITY);
	}
	else{
		/* flip IRE dest buffer to LCD driver */
		_overlay_lcd_display(datactx, datactx->cur_ctx);
		_EnArray(datactx->used_buf_ctx_array, datactx->cur_ctx, OVERLAY_QUEUE_CAPACITY);
	}
#ifdef QUEUE_TIME_MEASURE
	caculate_delta_time(overlay_time[4], overlay_time[5]);
#endif

    TRACE("%s x", __FUNCTION__);
    return 0;
}

static void *overlay_getBufferAddress(struct overlay_data_device_t *dev,
                               overlay_buffer_t buffer)
{
    /* this may fail (NULL) if this feature is not supported. In that case,
     * presumably, there is some other HAL module that can fill the buffer,
     * using a DSP for instance */
    struct overlay_data_context_t *ctx = (struct overlay_data_context_t *)dev;
    struct overlay_buffer_context_t *bufCtx = (struct overlay_buffer_context_t *)buffer;

    TRACE("%s e", __FUNCTION__);

    /* Allocate a piece of physical continuous memory , storing the data
     * passed by a virtual buffer */

	switch(ctx->format)
	{
	case FB_VMODE_YUV420PLANAR:

		bufCtx->bufHdr.vaddr = phy_cont_malloc((ALIGN16(ctx->width) * ALIGN16(ctx->height) * 3 >> 1), PHY_CONT_MEM_ATTR_DEFAULT);
        if (!bufCtx->bufHdr.vaddr) {
        	LOGE("%s: failed to allocate physical continuous memory", __FUNCTION__);
            return NULL;
        }

		bufCtx->bufHdr.len = (ALIGN16(ctx->width) * ALIGN16(ctx->height) * 3 / 2);
		break;

	case FB_VMODE_YUV422PACKED:
	case FB_VMODE_RGB565:

		bufCtx->bufHdr.vaddr = phy_cont_malloc((ALIGN16(ctx->width) * ALIGN16(ctx->height) * 2), PHY_CONT_MEM_ATTR_DEFAULT);
        if (!bufCtx->bufHdr.vaddr) {
        	LOGE("%s: failed to allocate physical continuous memory", __FUNCTION__);
            return NULL;
        }

 	   	bufCtx->bufHdr.len = (ALIGN16(ctx->width) * ALIGN16(ctx->height) * 2);
		break;

	case FB_VMODE_RGBA888:

		bufCtx->bufHdr.vaddr = phy_cont_malloc((ALIGN16(ctx->width) * ALIGN16(ctx->height) * 4), PHY_CONT_MEM_ATTR_DEFAULT);
        if (!bufCtx->bufHdr.vaddr) {
        	LOGE("%s: failed to allocate physical continuous memory", __FUNCTION__);
            return NULL;
        }

 	   	bufCtx->bufHdr.len = (ALIGN16(ctx->width) * ALIGN16(ctx->height) * 4);
		break;

    	default:
        	LOGE("%s: unsupported format %d", __FUNCTION__, ctx->format);
        	return NULL;
	}

    bufCtx->bufHdr.paddr = (void *)phy_cont_getpa(bufCtx->bufHdr.vaddr);
    bufCtx->bufHdr.free = phy_cont_free;
    bufCtx->bufHdr.user = bufCtx->bufHdr.vaddr;

    TRACE("%s x", __FUNCTION__);
    return bufCtx->bufHdr.vaddr;
}

static int overlay_getBufferCount(struct overlay_data_device_t *dev)
{
    TRACE("%s e", __FUNCTION__);
    TRACE("%s x", __FUNCTION__);
    return OVERLAY_QUEUE_CAPACITY;
}

static int overlay_data_close(struct hw_device_t *dev)
{
    TRACE("%s e", __FUNCTION__);
    struct overlay_data_context_t* datactx = (struct overlay_data_context_t*)dev;


	if (datactx) {
        /* flip last buffer to LCD driver */
        if(datactx->last_ctx){
            TRACE("%s disaply buf %p", __FUNCTION__,datactx->last_ctx );
            _overlay_lcd_display(datactx, datactx->last_ctx);
        }

        /* free all resources associated with this device here
         * in particular all pending overlay_buffer_t if needed.
         *
         * NOTE: overlay_handle_t passed in initialize() is NOT freed and
         * its file descriptors are not closed (this is the responsibility
         * of the caller).
         */
        LOGD("%s: close overlay data module(%p)", __FUNCTION__, datactx);

        if (datactx->fd_ovly > 0) {
	    close(datactx->fd_ovly);
        }

        _overlay_data_releaseContext(datactx);

        free(datactx);
        datactx = 0;
    }
    TRACE("%s x", __FUNCTION__);
    return 0;
}

/*****************************************************************************/

static int overlay_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
    int status = -EINVAL;
    TRACE("%s e, device name = %s\n", __FUNCTION__, name);
    if (!strcmp(name, OVERLAY_HARDWARE_CONTROL)) {
        struct overlay_control_context_t *dev;
        dev = (overlay_control_context_t*)malloc(sizeof(*dev));

        /* initialize our state here */
        memset(dev, 0, sizeof(*dev));

        /* initialize the procs */
        dev->device.common.tag     = HARDWARE_DEVICE_TAG;
        dev->device.common.version = 0;
        dev->device.common.module  = const_cast<hw_module_t*>(module);
        dev->device.common.close   = overlay_control_close;

        dev->device.get            = overlay_get;
        dev->device.createOverlay  = overlay_createOverlay;
        dev->device.destroyOverlay = overlay_destroyOverlay;
        dev->device.setPosition    = overlay_setPosition;
        dev->device.getPosition    = overlay_getPosition;
        dev->device.setParameter   = overlay_setParameter;
        dev->device.stage          = overlay_stage;
        dev->device.commit         = overlay_commit;

        *device = &dev->device.common;
        status = 0;
    } else if (!strcmp(name, OVERLAY_HARDWARE_DATA)) {
        struct overlay_data_context_t *dev;
	dev = (overlay_data_context_t*)malloc(sizeof(*dev));

        /* initialize our state here */
        memset(dev, 0, sizeof(*dev));

	/* initialize the procs */
        dev->device.common.tag       = HARDWARE_DEVICE_TAG;
        dev->device.common.version   = 0;
        dev->device.common.module    = const_cast<hw_module_t*>(module);
        dev->device.common.close     = overlay_data_close;

        dev->device.initialize       = overlay_initialize;
        dev->device.resizeInput      = overlay_resizeInput;
        dev->device.setCrop          = overlay_setCrop;
        dev->device.getCrop          = overlay_getCrop;
        dev->device.setParameter     = overlay_data_setParameter;
        dev->device.dequeueBuffer    = overlay_dequeueBuffer;
        dev->device.queueBuffer      = overlay_queueBuffer;
        dev->device.getBufferAddress = overlay_getBufferAddress;
        dev->device.getBufferCount   = overlay_getBufferCount;

        *device = &dev->device.common;
        status = 0;
    }
    TRACE("%s x\n", __FUNCTION__);
    return status;
}

