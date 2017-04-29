/*
 * INTEL CONFIDENTIAL
 * Copyright 2005-2006 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code (Material) are owned by Intel Corporation
 * or its suppliers or licensors.  Title to the Material remains with
 * Intel Corporation or its suppliers and licensors. The Material contains
 * trade secrets and proprietary and confidential information of Intel
 * or its suppliers and licensors. The Material is protected by worldwide
 * copyright and trade secret laws and treaty provisions. No part of the
 * Material may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without Intel's
 * prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.

 *(C) Copyright 2006 Marvell International Ltd.
 * All Rights Reserved
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/uio.h>
#include <linux/types.h>
#include <errno.h>

#include <linux/fb.h>
#include <string.h>
#include "ire_drv.h"
#include "ire_lib.h"
#ifdef BMM_BUF
#include "bmm_lib.h"
static char * bmm_buf;
#endif

/* android log header */
#include <utils/Log.h>

#ifdef __GNUC__
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)
#else
#define likely(x)	x
#define unlikely(x)	x
#endif

#define IRE_DEVICE_FILE	"/dev/ire"

/* #define DEBUG */

#ifdef  DEBUG
  #define pr_debug(fmt, arg...) printf(fmt, ##arg)
#else
  #define pr_debug(fmt, arg...) do { } while(0)
#endif


void * ire_buff_addr(struct ire_buff *buf)
{
	return (void *)(buf->addr);
}

size_t ire_buff_size(struct ire_buff *buf)
{
	return (size_t)(buf->size);
}

struct ire_context {
	int		ire_fd;
	struct ire_fmt	fmt;
};

/****************************************************************************
 * User Space Context Management
 ***************************************************************************/

struct ire_context *ire_create_context(void)
{
	int ire_fd;
	struct ire_context *ctx;
	struct ire_buff *sbuf;

	/* Allocate memory for the context structure */
	if ((ctx = malloc(sizeof(struct ire_context))) == NULL) {
		pr_debug("failed to allocate memory for context\n");
		return NULL;
	}
	memset(ctx, 0, sizeof(*ctx));

	/* Open the device and acquire a new context */
	if ((ctx->ire_fd = open(IRE_DEVICE_FILE, O_RDWR)) < 0) {
		LOGE("========= open ire in ire lib failed\n");
		pr_debug("failed to open 2D device file: %s\n",
				IRE_DEVICE_FILE);
		goto fail_free;
	}

	return ctx;

fail_close:
	close(ire_fd);

fail_free:
	free(ctx);
	return NULL;
}

void ire_free_context(struct ire_context *ctx)
{
	if (ctx == NULL) return;

	close(ctx->ire_fd);
	free(ctx);
}

/****************************************************************************
 * Graphics Buffer Management
 ***************************************************************************/

struct ire_buff *ire_create_buffer(
		struct ire_context *ctx,
		int width,
		int height,
		IRE_PIXEL_FORMAT pf,
		void *virt_addr)
{
	int rc = 0;
	struct ire_buff *buff;

	if ((buff = malloc(sizeof(struct ire_buff))) == NULL)
		return NULL;

	buff->format	= pf;
	buff->stride	= width * IRE_PIXFMT_BITS(pf)/8;

	buff->width	= width;
	buff->height	= height;
	buff->size	= buff->stride * buff->height;
	buff->addr	= virt_addr;
	buff->parent	= NULL;

	return buff;
}

void ire_destroy_buffer(struct ire_buff *srcbuf, struct ire_buff *dstbuf)
{
	if(srcbuf){
		free(srcbuf);
		srcbuf = NULL;
	}

	if(dstbuf){
		free(dstbuf);
		dstbuf = NULL;
	}
}

struct ire_buff *ire_alloc_buffer(
		struct ire_context *ctx,
		int width,
		int height,
		IRE_PIXEL_FORMAT pf)
{
	struct ire_mem_req mem_req;
	struct ire_buff *buff;

	buff = malloc(sizeof(struct ire_buff));
	if (buff == NULL)
		return NULL;

	mem_req.req_type = IRE_GRAPHICS_MEM;
	mem_req.req_size = width * height * IRE_PIXFMT_BITS(pf)/8;

	pr_debug("mem_req.req_type:%d\n", mem_req.req_type);
	pr_debug("mem_req.req_size:%d\n", mem_req.req_size);
	if (ioctl(ctx->ire_fd, IREIO_REQUEST_MEM, &mem_req) < 0)
		return NULL;

	buff->bus_addr	= mem_req.phys_addr;
	buff->format	= pf;
	buff->stride	= width * IRE_PIXFMT_BITS(pf)/8;

	buff->width	= width;
	buff->height	= height;
	buff->size	= buff->stride * buff->height;
	buff->addr	= (void *)mem_req.mmap_addr;
	buff->parent	= NULL;

	pr_debug("buff->bus_addr: 0x%08x\n", buff->bus_addr);
	pr_debug("buff->format: 0x%08x\n", buff->format);
	pr_debug("buff->stride: 0x%08x\n", buff->stride);
	pr_debug("buff->width: 0x%08x\n", buff->width);
	pr_debug("buff->height: 0x%08x\n", buff->height);
	pr_debug("buff->size: 0x%08x\n", buff->size);
	pr_debug("buff->addr: 0x%08x\n", buff->addr);

	return buff;
}

/* A sub-buffer represents a sub region of a buffer, it does not
 * have a graphics memory block associated. */
struct ire_buff* ire_sub_buffer(struct ire_buff *buff,
		int x, int y,
		int w, int h)
{
	struct ire_buff *sbuff;
	unsigned long offset;

	sbuff = malloc(sizeof(struct ire_buff));
	if (sbuff == NULL)
		return NULL;

	offset = (y * buff->stride) + x * IRE_PIXFMT_BITS(buff->format)/8;
	sbuff->format	= buff->format;
	sbuff->stride	= buff->stride;

	sbuff->width	= w;
	sbuff->height	= h;

	sbuff->addr	= buff->addr + offset;
	sbuff->parent	= buff;

	return sbuff;
}

int __drv_angle(int rot_angle)
{
	int ret;
	 /* 90 degree: 0 in driver; 1 in ire-lib
	 * 270 degree: 1 in driver; 3 in ire- lib
	 * 180 degree: 2 in driver; 2 in ire- lib
	 */
	switch(rot_angle){
		case IRE_LIB_ROT_90:
			ret = IRE_ROT_90;
		break;
		case IRE_LIB_ROT_270:
			ret = IRE_ROT_270;
		break;
		case IRE_LIB_ROT_180:
			ret = IRE_ROT_180;
		break;
		default:
			ret = -EINVAL;
		break;
	}
	return ret;
}

int ire_set_format(struct ire_context *ctx, unsigned int width, unsigned int height,
		unsigned int format, unsigned int rot_angle)
{
	int drv_angle = __drv_angle(rot_angle);
	if(drv_angle < 0)
		return -EINVAL;

	ctx->fmt.rot_angle = drv_angle;
	ctx->fmt.fmt = format;
	ctx->fmt.width = width;
	ctx->fmt.height = height;
	ctx->fmt.y_pitch = width;
	ctx->fmt.uv_pitch = width/2;
	ctx->fmt.yp0_pitch = height;
	ctx->fmt.uvp0_pitch = height/2;
	return ioctl(ctx->ire_fd, IREIO_S_FMT, &ctx->fmt);
}


/* FIXME: all sub buffer shall be freed as well
 */
void ire_free_buffer(struct ire_context *ctx, struct ire_buff *buff)
{
	ioctl(ctx->ire_fd, IREIO_RELEASE_MEM, buff->addr);
	free(buff);
}

struct ire_buff *ire_get_primary(void)
{
	static int fbdev = 0;
	static struct ire_buff primary;
	struct fb_fix_screeninfo fix;
	struct fb_var_screeninfo var;
	void *addr;

	if (fbdev <= 0) {
		fbdev = open("/dev/fb0", O_RDWR);
		if (fbdev <= 0)
			return NULL;

		ioctl(fbdev, FBIOGET_FSCREENINFO, &fix);
		ioctl(fbdev, FBIOGET_VSCREENINFO, &var);
		primary.bus_addr  = fix.smem_start;
		primary.format    = IRE_PIXFMT_RGB565;
		primary.width     = var.xres;
		primary.height    = var.yres;
		primary.step      = var.bits_per_pixel / 8;
		primary.stride    = var.xres * primary.step;

		addr = mmap(NULL, fix.smem_len, PROT_READ | PROT_WRITE,
				MAP_SHARED, fbdev, 0);
		if (addr == MAP_FAILED) {
			fbdev = 0;
			return NULL;
		}
		primary.addr   = addr;
		primary.parent = NULL;
	}

	return &primary;
}

/****************************************************************************
 * Instruction Execution Management
 ***************************************************************************/

int ire_submit(struct ire_context *ctx, struct ire_buff *src, struct ire_buff *dst)
{
	struct ire_submit_req req;
	unsigned int ret = 0;

	req.size        = src->size;
	req.src.y_paddr = (unsigned long) src->addr;
	req.src.u_paddr = req.src.y_paddr + ctx->fmt.width*ctx->fmt.height;
	req.src.v_paddr = req.src.u_paddr + ctx->fmt.width*ctx->fmt.height*IRE_PIXFMT_UV_FACTOR(src->format)/8;

	req.dst.y_paddr = (unsigned long) dst->addr;
	req.dst.u_paddr = req.dst.y_paddr + ctx->fmt.width*ctx->fmt.height;
	req.dst.v_paddr = req.dst.u_paddr + ctx->fmt.width*ctx->fmt.height*IRE_PIXFMT_UV_FACTOR(src->format)/8;
	if ((ret = ioctl(ctx->ire_fd, IREIO_SUBMIT, &req)) == 0)
		return req.buf_id;
	else
	ret = -EAGAIN;

	return ret;
}

int ire_desubmit(struct ire_context *ctx, unsigned int buf_id)
{
	int ret;

	if ((ret = ioctl(ctx->ire_fd, IREIO_DESUBMIT, buf_id)) == 0)
		return 0;
	else
		ret = -EAGAIN;

	return ret;
}

int ire_enqueue(struct ire_context *ctx, int buf_id)
{
	return ioctl(ctx->ire_fd, IREIO_ENQUEUE, (void*)buf_id);
}

int ire_dequeue(struct ire_context *ctx, int buf_id)
{
	return ioctl(ctx->ire_fd, IREIO_DEQUEUE, (void*)buf_id);
}

int ire_enable(struct ire_context *ctx)
{
	return ioctl(ctx->ire_fd, IREIO_ENABLE, NULL);
}

int ire_disable(struct ire_context *ctx)
{
	return ioctl(ctx->ire_fd, IREIO_DISABLE, NULL);
}

/* ******ire wrapper**********/

/* Set whether we directly use src as physical continuous buffer */
void ire_set_use_srcbuf(struct ire_info *ire, int val)
{
	ire->use_srcbuf = val;
}

int ire_get_use_srcbuf(struct ire_info *ire)
{
	return ire->use_srcbuf;
}

void ire_set_degree(struct ire_info *ire, int degree)
{
	ire->rotate_degree = degree;
}

int ire_get_degree(struct ire_info *ire)
{
	return ire->rotate_degree;
}

int ire_open(struct ire_info *ire, struct v_frame *src)
{
	int i;
	if(1 != ire->use_srcbuf){
		LOGE(" %s %d only user point is supported \n", __FUNCTION__, __LINE__);
		return -1;
	}

	/* create context */
	ire->ctx = ire_create_context();
	if(!ire->ctx)
		return -1;

	memset(&(ire->sbuf), 0, sizeof(struct ire_buffer *));
	memset(&(ire->dbuf), 0, sizeof(struct ire_buffer *));

	/* alloc src buffer */
#ifndef BMM_BUF
	if(! ire->use_srcbuf){
		ire->sbuf = ire_alloc_buffer(ire->ctx, src->width, src->height, IRE_PIXFMT_YUV420_PLANAR);
	}
#else
	if(! ire->use_srcbuf){
		bmm_buf = bmm_malloc((src->height * src->width * 3) >> 1, BMM_ATTR_DEFAULT);
		if(!bmm_buf)
			LOGE("================= bmm malloc err ===============\n");
		return -1;
	}
#endif
	return 0;
}

int ire_process(struct ire_info *ire, struct v_frame *src, struct v_frame *dst)
{
	int pair_id;
	int ret = 0;
	if(1 != ire->use_srcbuf){
		LOGE(" %s %d only user point is supported \n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}
	if(! ire->ctx){
		LOGE(" %s %d ire->ctx is NULL \n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}

	if(! ire->rotate_degree){
		LOGE(" %s %d ire->rotate_degree is 0-degree \n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}
	/* fill src buffer */
#ifndef BMM_BUF
	if(! ire->use_srcbuf){
		memcpy(ire_buff_addr(ire->sbuf), src->start[0], ire_buff_size(ire->sbuf));
	}else{
#else
	if(! ire->use_srcbuf){
		/* Use bmm buffer directly */
		memcpy(bmm_buf, src->start[0], src->width * src->width *3/2);
		ire->sbuf = ire_create_buffer(ire->ctx, src->width,
			src->height, IRE_PIXFMT_YUV420_PLANAR,
			bmm_buf);
		if(! ire->sbuf){
			LOGE(" %s %d ire->sbuf is NULL \n", __FUNCTION__, __LINE__);
			ret = -1;
			goto out;
			}
	}else{
#endif
		/* Use source buffer directly */
		ire->sbuf = ire_create_buffer(ire->ctx, src->width,
			src->height, IRE_PIXFMT_YUV420_PLANAR,
			src->start[0]);
		if(! ire->sbuf){
			LOGE(" %s %d ire->sbuf is NULL \n", __FUNCTION__, __LINE__);
			ret = -1;
			goto out;
			}
	}

	/* create dst buffer from externel pointer, for overlay buffer is phy continous */
	ire->dbuf = ire_create_buffer(ire->ctx, src->height,
			src->width, IRE_PIXFMT_YUV420_PLANAR,
			dst->start[0]);
		if(!ire->dbuf){
			LOGE("create dst buf fail\n");
			ret = -1;
			goto out;
		}

	/* set format */
	if(ire_set_format(ire->ctx, src->width, src->height, IRE_PIXFMT_YUV420_PLANAR, ire->rotate_degree)){
		ret = -1;
		goto out;
	}

	/* submit buffer info to driver */
	pair_id = ire_submit(ire->ctx, ire->sbuf, ire->dbuf);
	if(pair_id < 0){
		LOGE(" %s %d  ire_submit failed\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}
	//printf("pair_id:%d\n", pair_id);

	/* enqueue buffers to driver */
	ret = ire_enqueue(ire->ctx, pair_id);
	if(0 != ret){
		LOGE(" %s %d  ire_enqueue failed\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}

	/* commit operation and block till finish */
	ret = ire_enable(ire->ctx);
	if(0 != ret){
		LOGE(" %s %d  ire_enable failed\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}

	/* dequeue buffers for output */
	ret = ire_dequeue(ire->ctx, pair_id);
	if(0 != ret){
		LOGE(" %s %d  ire_dequeue failed\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}
	/* desubmit buffer info to driver */
	ret = ire_desubmit(ire->ctx, pair_id);
	if(0 != ret){
		LOGE(" %s %d  ire_desubmit failed\n", __FUNCTION__, __LINE__);
		ret = -1;
		goto out;
	}

	/* disable IRE */
	ire_disable(ire->ctx);
out:
	ire_destroy_buffer(ire->sbuf, ire->dbuf);
	return ret;
}

int ire_close(struct ire_info *ire)
{
	int buf_id;
	if(1 != ire->use_srcbuf){
		LOGE(" %s %d only user point is supported \n", __FUNCTION__, __LINE__);
		return -1;
	}
	if(! ire->ctx)
		return 0;

	/* free context */
	ire_free_context(ire->ctx);

	ire->ctx = 0;
#ifdef BMM_BUF
	if(! ire->use_srcbuf){
		bmm_free(bmm_buf);
	}
#endif
	return 0;
}


