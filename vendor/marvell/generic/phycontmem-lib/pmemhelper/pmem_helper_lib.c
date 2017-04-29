/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/android_pmem.h>

#include "pmem_helper_lib.h"

#if 0
#define pmem_helper_echo(...)	printf(__VA_ARGS__)
#else
#define pmem_helper_echo(...)
#endif

struct pmem_handle_mrvl* pmem_malloc(int size, const char* devname)
{
	struct pmem_handle_mrvl* pmem;
	struct pmem_region pr;
	int rlt = 0;

	pmem = (struct pmem_handle_mrvl*)malloc( sizeof(struct pmem_handle_mrvl) );
	if( NULL == pmem ) {
		pmem_helper_echo("malloc in %s(line %d) fail\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	memset( pmem, 0, sizeof(struct pmem_handle_mrvl) );
	pmem->fd = open( devname, O_RDWR );
	if( pmem->fd < 0 ) {
		pmem_helper_echo("open %s in %s(line %d) fail, ret fd %d\n", devname, __FUNCTION__, __LINE__, pmem->fd);
		goto pmem_malloc_fail0;
	}

	pmem->va = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, pmem->fd, 0);
	if( pmem->va == MAP_FAILED ) {
		pmem_helper_echo("mmap in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, pmem->va);
		goto pmem_malloc_fail1;
	}
	pmem->size = size;

	rlt = ioctl( pmem->fd, PMEM_GET_PHYS, (unsigned long)&pr);
	if( rlt < 0 ) {
		pmem_helper_echo("PMEM_GET_PHYS in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, rlt);
		goto pmem_malloc_fail2;
	}

	pmem->pa = (void*)pr.offset;
	return pmem;

pmem_malloc_fail2:
	munmap( pmem->va, pmem->size );
pmem_malloc_fail1:
	close( pmem->fd );
pmem_malloc_fail0:
	free( pmem );
	return NULL;
}

int pmem_free(struct pmem_handle_mrvl* handle)
{
	if(handle == NULL) {
		return -1;
	}
	if(handle->fd < 0) {
		return -2;
	}
	munmap( handle->va, handle->size );
	close(handle->fd);
	free(handle);
	return 0;
}

void pmem_flush_cache(int pmem_fd, unsigned long offset, unsigned long size, int dir)
{
	struct pmem_sync_region psr;
	int ret;
	if(pmem_fd < 0)
		return;
	psr.region.offset = offset;
	psr.region.len = size;

	if(dir == PMEM_FLUSH_BIDIRECTION) {
		psr.dir = DMA_BIDIRECTIONAL;
	}else if(dir == PMEM_FLUSH_TO_DEVICE) {
		psr.dir = DMA_TO_DEVICE;
	}else if(dir == PMEM_FLUSH_FROM_DEVICE) {
		psr.dir = DMA_FROM_DEVICE;
	}else{
		return;
	}

	if (psr.dir == DMA_BIDIRECTIONAL) {
		ret = ioctl(pmem_fd, PMEM_CACHE_FLUSH, (unsigned long)&psr.region);
		if( ret < 0 ) {
			pmem_helper_echo("PMEM_CACHE_FLUSH in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, ret);
		}
	} else {
		ret = ioctl(pmem_fd, PMEM_MAP_REGION, (unsigned long)&psr);
		if( ret < 0 ) {
			pmem_helper_echo("PMEM_CACHE_FLUSH in %s(line %d) fail, ret %d\n", __FUNCTION__, __LINE__, ret);
		}
	}
}

