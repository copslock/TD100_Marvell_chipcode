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

#ifndef IRE_LIB_H
#define IRE_LIB_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ire_drv.h"

#define NR_BUFFER 3

struct ire_context;

struct ire_buff {
	uint32_t	bus_addr; /* bus address */
	IRE_PIXEL_FORMAT	format;   /* pixel format */
	uint32_t	stride;   /* size of a scan line */
	uint32_t	step;     /* size of a pixel */

	void *		addr;     /* address application can access */
	int		width;    /* width & height of the rectangle */
	int		height;   /* this buffer represent */
	size_t		size;	  /* size of the buffer */

	struct ire_buff *parent;  /* != NULL if this is a sub buffer */
};

/* Used for YCrCb data, it is stored in three areas */
struct v_frame {
	int height;
	int width;
	void *  start[3];
	size_t  length[3];
};

struct ire_info{
	struct ire_buff *sbuf;
	struct ire_buff *dbuf;
	struct ire_context *ctx;
	int rotate_degree;
	int use_srcbuf;
};

typedef enum {
	IRE_LIB_ROT_0	= 0,
	IRE_LIB_ROT_90	= 1,
	IRE_LIB_ROT_180	= 2,
	IRE_LIB_ROT_270	= 3,
} IRE_LIB_ROTATION;

/* returns the address of the graphics buffer that application can access */
void * ire_buff_addr(struct ire_buff *);
size_t ire_buff_size(struct ire_buff *);

int ire_submit(struct ire_context *, struct ire_buff *, struct ire_buff *);
int ire_desubmit(struct ire_context *, unsigned int buf_id);
int ire_set_format(struct ire_context *ctx, unsigned int width, unsigned int height,
		unsigned int format, unsigned int rot_angle);
int ire_enqueue(struct ire_context *, int buf_id);
int ire_dequeue(struct ire_context *, int buf_id);
int ire_enable(struct ire_context *);
int ire_disable(struct ire_context *);
struct ire_context *ire_create_context(void);
void ire_free_context(struct ire_context *);
struct ire_buff *ire_create_buffer(
		struct ire_context *ctx,
		int width,
		int height,
		IRE_PIXEL_FORMAT pf,
		void *virt_addr);

void ire_destroy_buffer(
		struct ire_buff *,
		struct ire_buff *);

struct ire_buff *ire_alloc_buffer(
		struct ire_context *ctx,
		int width,
		int height,
		IRE_PIXEL_FORMAT pf);

struct ire_buff *ire_sub_buffer(
		struct ire_buff *buff,
		int x, int y,
		int w, int h);

void ire_free_buffer(
		struct ire_context *ctx,
		struct ire_buff *);

struct ire_buff *ire_get_primary(void);

/*ire wrapper*/
void ire_set_use_srcbuf(struct ire_info *ire, int val);
int ire_get_use_srcbuf(struct ire_info *ire);
void ire_set_degree(struct ire_info *ire, int degree);
int ire_get_degree(struct ire_info *ire);
int ire_open(struct ire_info *ire, struct v_frame *src);
int ire_process(struct ire_info *ire, struct v_frame *src, struct v_frame *dst);
int ire_close(struct ire_info *ire);

#ifdef __cplusplus
}
#endif
#endif /* IRE_LIB_H */
