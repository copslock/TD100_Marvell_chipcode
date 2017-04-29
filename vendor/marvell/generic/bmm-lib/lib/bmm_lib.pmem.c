/*
 *  bmm_lib.c
 *
 *  Buffer Management Module for vMeta
 *
 *  based on Android PMEM
 *
 *  Steve Guo (steve.guo@marvell.com)

 *(C) Copyright 2007 Marvell International Ltd.
 * All Rights Reserved
 */

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/android_pmem.h>
#include <stdio.h>
#include <stdlib.h>
#define LOG_TAG "bmm-lib"
#include <utils/Log.h>

#include "bmm_lib.h"

//#define DEBUG

#ifdef DEBUG
  #define pr_debug(fmt, arg...) LOGD(fmt, ##arg)
#else
  #define pr_debug(fmt, arg...) do { } while(0)
#endif

#define pr_error(fmt, arg...) LOGE(fmt, ##arg)
#define NO_IMPLEMENTED LOGE("%s is not implemented yet.", __FUNCTION__)

#define PMEM_DEVICE_FILE "/dev/pmem_adsp"


/* We recorded all malloced physical address into a list. Due to the implementation of
pmem, we can only open a new file to malloc some memory and close it to free memory. So
we also record the opened fd associated with physical and virtual address map. */
struct memory_item
{
	int fd;
	void *paddr;
	void *vaddr;
	int size;
	struct memory_item *next;
};

static struct memory_item *header = NULL;

int bmm_init()
{
	pr_debug("bmm_init");
	header = NULL;
	return 0;
}

void bmm_exit()
{
	struct memory_item *cur = header;
	struct memory_item *next = NULL;
	pr_debug("bmm_exit");
	while(cur != NULL)
	{
		next = cur->next;
		munmap(cur->vaddr, cur->size);
		close(cur->fd);
		free(cur);
		cur = next;
	}
}

void *bmm_malloc(unsigned long size, int attr)
{
	void *vaddr;
	int pmem_fd = 0;
	struct memory_item *item = NULL;
	struct pmem_region region;
	int ret;

	if(size == 0)
		return NULL;

	pr_debug("bmm_malloc %d", size);

        /* attempt to open the PMEM driver */
	pmem_fd = open(PMEM_DEVICE_FILE, O_RDWR);
	if(pmem_fd <=0)
	{
		pr_error("cannot open %s file", PMEM_DEVICE_FILE);	
		return NULL;
	}

	vaddr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, pmem_fd, 0);
	if((int)vaddr == -1)
	{
		pr_error("mmap %d failed, most likely no memory", size);
		close(pmem_fd);
		return NULL;
	}

	ret = ioctl(pmem_fd, PMEM_GET_PHYS, &region);
        if(ret != 0)
        {
                pr_error("call PMEM_GET_PHYS failed");
		munmap(vaddr, size);
                close(pmem_fd);
                return NULL;
        }

	item = (struct memory_item*)malloc(sizeof(struct memory_item));
	memset(item, 0 , sizeof(struct memory_item));
	item->fd = pmem_fd;
	item->paddr = (void*)region.offset;
	item->vaddr = vaddr;
	item->size = size;

	if(header == NULL)
	{
		header = item;
	}
	else
	{
		item->next = header;
		header = item;
	}
	return vaddr;
}

void bmm_free(void *vaddr)
{
	pr_debug("bmm_free");
	struct memory_item *cur = header;
	struct memory_item *prev = NULL;
	struct memory_item *next = NULL;
	while(cur != NULL)
	{
		next = cur->next;
		if(cur->vaddr == vaddr)
		{
			munmap(cur->vaddr, cur->size);
			close(cur->fd);
			if(prev == NULL)
			{
				header = next;
			}
			else
			{
				prev->next = next;
			}
			free(cur);
			return;
		}
		prev = cur;
		cur = next;
	}
}

void *bmm_attach(void *paddr, unsigned long len)
{
	NO_IMPLEMENTED;
	return NULL;
}

void bmm_detach(void *vaddr, unsigned long len)
{
	NO_IMPLEMENTED;
}

void *bmm_get_vaddr(void *paddr)
{
	pr_debug("bmm_get_vaddr");
	struct memory_item *cur = header;
	while(cur != NULL)
	{
		if(cur->paddr == paddr)
			return cur->vaddr;
		cur = cur->next;
	}
	return NULL;
}

void *bmm_get_paddr(void *vaddr)
{
	pr_debug("bmm_get_paddr");
	struct memory_item *cur = header;
	while(cur != NULL)
	{
		if(cur->vaddr == vaddr)
			return cur->paddr;
		cur = cur->next;
	}
	return NULL;
}

unsigned long bmm_get_mem_size(void *vaddr)
{
	struct memory_item *cur = header;
	while(cur != NULL)
	{
		if(cur->vaddr == vaddr)
			return cur->size;
		cur = cur->next;
	}
	return 0;
}

int bmm_get_mem_attr(void *vaddr)
{
	NO_IMPLEMENTED;
	return 0;
}

int bmm_set_mem_attr(void *vaddr, int attr)
{
	NO_IMPLEMENTED;
	return 0;
}

unsigned long bmm_get_total_space()
{
	int pmem_fd = 0;
	struct pmem_region region;
	int ret;

	/* attempt to open the PMEM driver */
	pmem_fd = open(PMEM_DEVICE_FILE, O_RDWR);
	if(pmem_fd <=0)
	{
		pr_error("cannot open %s file", PMEM_DEVICE_FILE);
		return 0;
	}

	ret = ioctl(pmem_fd, PMEM_GET_TOTAL_SIZE, &region);
	if(ret != 0)
	{
		pr_error("call PMEM_GET_TOTAL_SIZE failed");
		close(pmem_fd);
		return 0;
	}	
	close(pmem_fd);
	return region.len;
}

unsigned long bmm_get_free_space()
{
	NO_IMPLEMENTED;
	return 0;
}

unsigned long bmm_get_allocated_space()
{
        NO_IMPLEMENTED;
        return 0;
}

void bmm_flush_cache(void *vaddr, int dir)
{
	NO_IMPLEMENTED;
}

void *bmm_dma_memcpy(void *dest, const void *src, size_t n)
{
	NO_IMPLEMENTED;
	return NULL;
}

void bmm_dma_sync()
{
	NO_IMPLEMENTED;
}

void bmm_flush_cache_range(void *start, size_t size, int direction)
{
	NO_IMPLEMENTED;
}

void bmm_dump()
{
	NO_IMPLEMENTED;
}

