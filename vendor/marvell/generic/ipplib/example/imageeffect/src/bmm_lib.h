/*
 *  bmm_lib.h
 *
 *  Buffer Management Module
 *
 *  User level BMM Defines/Globals/Functions
 *
 *  Li Li (lea.li@marvell.com)

 *(C) Copyright 2007 Marvell International Ltd.
 * All Rights Reserved
 */

#if !defined (__BMM_LIB_H__)
#define __BMM_LIB_H__

#define BMM_MINOR		(94)

typedef struct {
	unsigned long input;		/* the starting address of the block of memory */
	unsigned long output;		/* the starting address of the block of memory */
	unsigned long length;		/* the length of the block of memory */
	unsigned long arg;		/* the arg of cmd */
} ioctl_arg_t;

#define BMEM_IOCTL_MAGIC 'G'
/* ioctl commands */
#define BMM_MALLOC		_IOWR(BMEM_IOCTL_MAGIC, 0, ioctl_arg_t)
#define BMM_FREE		_IOWR(BMEM_IOCTL_MAGIC, 1, ioctl_arg_t)
#define BMM_GET_VIRT_ADDR	_IOWR(BMEM_IOCTL_MAGIC, 2, ioctl_arg_t)
#define BMM_GET_PHYS_ADDR	_IOWR(BMEM_IOCTL_MAGIC, 3, ioctl_arg_t)
#define BMM_GET_MEM_ATTR	_IOWR(BMEM_IOCTL_MAGIC, 4, ioctl_arg_t)
#define BMM_SET_MEM_ATTR	_IOWR(BMEM_IOCTL_MAGIC, 5, ioctl_arg_t)
#define BMM_GET_MEM_SIZE	_IOWR(BMEM_IOCTL_MAGIC, 6, ioctl_arg_t)
#define BMM_GET_TOTAL_SPACE	_IOWR(BMEM_IOCTL_MAGIC, 7, ioctl_arg_t)
#define BMM_GET_FREE_SPACE	_IOWR(BMEM_IOCTL_MAGIC, 8, ioctl_arg_t)
#define BMM_FLUSH_CACHE		_IOWR(BMEM_IOCTL_MAGIC, 9, ioctl_arg_t)
#define BMM_DMA_MEMCPY		_IOWR(BMEM_IOCTL_MAGIC, 10, ioctl_arg_t)
#define BMM_DMA_SYNC		_IOWR(BMEM_IOCTL_MAGIC, 11, ioctl_arg_t)
#define BMM_CONSISTENT_SYNC	_IOWR(BMEM_IOCTL_MAGIC, 12, ioctl_arg_t)
#define BMM_DUMP		_IOWR(BMEM_IOCTL_MAGIC, 13, ioctl_arg_t)
#define BMM_GET_ALLOCATED_SPACE	_IOWR(BMEM_IOCTL_MAGIC, 14, ioctl_arg_t)
#define BMM_GET_KERN_PHYS_ADDR	_IOWR(BMEM_IOCTL_MAGIC, 15, ioctl_arg_t)

/* ioctl arguments: memory attributes */
#define BMM_ATTR_DEFAULT	(0)		/* cacheable bufferable */
#define BMM_ATTR_WRITECOMBINE	(1 << 0)	/* non-cacheable & bufferable */
#define BMM_ATTR_NONCACHED	(1 << 1)	/* non-cacheable & non-bufferable */

/* Note: extra attributes below are not supported yet! */
#define BMM_ATTR_HUGE_PAGE	(1 << 2)	/* 64KB page size */
#define BMM_ATTR_WRITETHROUGH	(1 << 3)	/* implies L1 Cacheable */
#define BMM_ATTR_L2_CACHEABLE	(1 << 4)	/* implies L1 Cacheable */

/* obsolete */
#define BMM_ATTR_NONBUFFERABLE	(1 << 5)	/* non-bufferable */
#define BMM_ATTR_NONCACHEABLE	(1 << 6)	/* non-cacheable */

/* ioctl arguments: cache flush direction */
#define BMM_DMA_BIDIRECTIONAL	(0)		/* DMA_BIDIRECTIONAL */
#define BMM_DMA_TO_DEVICE	(1)		/* DMA_TO_DEVICE */
#define BMM_DMA_FROM_DEVICE	(2)		/* DMA_FROM_DEVICE */
#define BMM_DMA_NONE		(3)		/* DMA_NONE */

#define BMM_DEVICE_FILE	"/dev/bmm"

#if defined (__cplusplus)
extern "C" {
#endif

int bmm_init();
void bmm_exit();
void *bmm_malloc(unsigned long size, int attr);
void bmm_free(void *vaddr);
void *bmm_attach(void *paddr, unsigned long len);
void bmm_detach(void *vaddr, unsigned long len);
void *bmm_get_vaddr(void *paddr);
void *bmm_get_paddr(void *vaddr);
void *bmm_get_kern_paddr(void *vaddr, unsigned long size);
int bmm_get_mem_attr(void *vaddr);
int bmm_set_mem_attr(void *vaddr, int attr);	/* Not supported yet */
unsigned long bmm_get_mem_size(void *vaddr);
unsigned long bmm_get_total_space();
unsigned long bmm_get_free_space();
unsigned long bmm_get_allocated_space();
void bmm_flush_cache(void *vaddr, int dir);
void *bmm_dma_memcpy(void *dest, const void *src, size_t n);	/* Obsolete */
void bmm_dma_sync();	/* Obsolete */
void bmm_flush_cache_range(void *start, size_t size, int direction);
void bmm_dump();	/* for debugging */

#if defined (__cplusplus)
}
#endif

#endif /* __BMM_LIB_H__ */

