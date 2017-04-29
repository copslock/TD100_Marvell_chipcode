/*
 *  bmm_test.c
 *
 *  Buffer Management Module
 *
 *  User test application
 *
 *  Li Li (lea.li@marvell.com)

 *(C) Copyright 2007 Marvell International Ltd.
 * All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include "bmm_lib.h"

static int debug = 0;

int test_count = 0;
int pass_count = 0;
int fail_count = 0;

#define PRINT_TEST_NAME		printf("Testing %d: %s()\n", test_count++, __FUNCTION__)
#define PRINT_TEST_RESULT(x) \
	do { \
		if(!x) \
			pass_count++, printf("\tPassed.\n"); \
		else \
			fail_count++, printf("\tFailed.\n"); \
	} while(0)
#define PRINT_TEST_RESULTS()	printf("Passed/Failed = %d/%d\n", pass_count, fail_count)

int test_bmm_init()
{
	int ret;

	PRINT_TEST_NAME;

	ret = bmm_init();
	printf("\tbmm_init() return %d\n", ret);
	
	return (ret <= 0);
}

int test_bmm_get_space()
{
	unsigned long total_size;
	unsigned long free_size;

	PRINT_TEST_NAME;

	total_size = bmm_get_total_space();
	free_size = bmm_get_free_space();

	printf("\tbmm_get_total_space() return %ldKB\n", total_size/1024);
	printf("\tbmm_get_free_space() return %ldKB\n", free_size/1024);

	return (total_size <= 0 || total_size != free_size);
}

int test_bmm_malloc(unsigned long size, int attr)
{
	void *vaddr;
	unsigned long allocated_size1, free_size1;
	unsigned long allocated_size2, free_size2;
	unsigned long allocated_size3, free_size3;
	int off = 0;
	PRINT_TEST_NAME;

	allocated_size1 = bmm_get_allocated_space();
	free_size1 = bmm_get_free_space();
	vaddr = bmm_malloc(size, attr);
	if(vaddr == NULL) {
		printf("\tno enough memory\n");
		return -1;
	}
	for(off = 0; off < size; off += 1024*1024){
		void *get_paddr = bmm_get_paddr(vaddr+off);
		void *get_vaddr = bmm_get_vaddr(get_paddr);
		int get_attr = bmm_get_mem_attr(get_vaddr);
		printf("\toff = 0x%x, vaddr=0x%p, get_vaddr=0x%p, get_paddr=0x%p\n", off, vaddr+off, get_vaddr, get_paddr);
		printf("\tset_attr=0x%x, get_attr=0x%x\n", attr, get_attr);
		if(vaddr+off != get_vaddr)
			return -1;
	}

	allocated_size2 = bmm_get_allocated_space();
	free_size2 = bmm_get_free_space();
	bmm_free(vaddr);
	allocated_size3 = bmm_get_allocated_space();
	free_size3 = bmm_get_free_space();

	printf("\tbmm_malloc(%ldKB) return 0x%08lx\n", size/1024, (unsigned long)vaddr);
	printf("\tbmm_get_allocated_space() return %ldKB %ldKB %ldKB\n", 
		allocated_size1/1024, allocated_size2/1024, allocated_size3/1024);
	printf("\tbmm_get_free_space() return %ldKB %ldKB %ldKB\n", 
		free_size1/1024, free_size2/1024, free_size3/1024);

	return ((int)vaddr <= 0 || 
		allocated_size2 - allocated_size1 != size ||
		allocated_size1 != allocated_size3 ||
		free_size1 - free_size2 != size || 
		free_size1 != free_size3);
}

int test_bmm_dma_memcpy(unsigned long size)
{
	char *src;
	char *dst;
	unsigned long i;
	unsigned long free_size1;
	unsigned long free_size2;
	unsigned long free_size3;
	int ret = 0;

	PRINT_TEST_NAME;

	free_size1 = bmm_get_free_space();
	src = bmm_malloc(size, BMM_ATTR_DEFAULT);
	dst = bmm_malloc(size, BMM_ATTR_DEFAULT);
	memset(src, 0, size);
	memset(dst, 0, size);
	free_size2 = bmm_get_free_space();
	if(src == NULL) {
		printf("\tno enough memory\n");
		return -1;
	}
	if(dst == NULL) {
		printf("\tno enough memory\n");
		bmm_free(src);
		return -2;
	}
	for(i=0; i<size; i++)
		src[i] = dst[size-i-1] = i;
	bmm_flush_cache(src, BMM_DMA_TO_DEVICE);
	bmm_flush_cache(dst, BMM_DMA_TO_DEVICE);
	bmm_dma_memcpy(dst, src, size);
	bmm_dma_sync();
	bmm_flush_cache(dst, BMM_DMA_FROM_DEVICE);
	for(i=0; i<size; i++) {
		if(src[i] != dst[i]) {
			ret--;
			if(debug)
				printf("\t[%ld]: %d != %d\n", i, src[i], dst[i]);
			else
				break;
		}
	}

	bmm_free(src);
	bmm_free(dst);
	free_size3 = bmm_get_free_space();

	printf("\tbmm_dma_memcpy(%ld/%ld) = %d\n", size, i, ret);
	printf("\tbmm_get_free_space() return %ldKB %ldKB %ldKB\n", 
		free_size1/1024, free_size2/1024, free_size3/1024);

	if(ret)
		return ret;

	return (free_size1 - free_size2 != size << 1 || free_size1 != free_size3);
}

int test_bmm_dma_memcpy_range(unsigned long size)
{
	char *src;
	char *dst;
	unsigned long i, offset;
	unsigned long free_size1;
	unsigned long free_size2;
	unsigned long free_size3;
	int ret = 0;

	PRINT_TEST_NAME;

	free_size1 = bmm_get_free_space();
	src = bmm_malloc(size, BMM_ATTR_DEFAULT);
	dst = bmm_malloc(size, BMM_ATTR_DEFAULT);
	memset(src, 0, size);
	memset(dst, 0, size);
	free_size2 = bmm_get_free_space();
	if(src == NULL) {
		printf("\tno enough memory\n");
		return -1;
	}
	if(dst == NULL) {
		printf("\tno enough memory\n");
		bmm_free(src);
		return -2;
	}
	for(i=0; i<size; i++)
		src[i] = dst[size-i-1] = i;
	for(offset=0; offset+size/2<size && offset<8096; offset+=128) {
		bmm_flush_cache_range(src+offset, size/2, BMM_DMA_TO_DEVICE);
		bmm_flush_cache_range(dst+offset, size/2, BMM_DMA_TO_DEVICE);
		bmm_dma_memcpy(dst+offset, src+offset, size/2);
		bmm_dma_sync();
		bmm_flush_cache_range(dst+offset, size/2, BMM_DMA_FROM_DEVICE);
		for(i=offset; i<size/2; i++) {
			if(src[i] != dst[i]) {
				ret--;
				if(debug)
					printf("\t[%ld]: %d != %d\n", i, src[i], dst[i]);
				else
					break;
			}
		}
	}

	bmm_free(src);
	bmm_free(dst);
	free_size3 = bmm_get_free_space();

	printf("\tbmm_dma_memcpy(%ld/%ld) = %d\n", size, i, ret);
	printf("\tbmm_get_free_space() return %ldKB %ldKB %ldKB\n", 
		free_size1/1024, free_size2/1024, free_size3/1024);

	if(ret)
		return ret;

	return (free_size1 - free_size2 != size << 1 || free_size1 != free_size3);
}

long gettickcount(void)
{
	struct timeval g_tv;
	struct timezone g_tz;

	gettimeofday(&g_tv, &g_tz);

	return g_tv.tv_sec * 1000000 + g_tv.tv_usec;
}

int test_bmm_flush_user(long size)
{
	char *src;
	unsigned long offset;
	long t1 = 0, t2 = 0;

	PRINT_TEST_NAME;

	src = malloc(size);
	if (src == NULL) {
		printf("\tno enough memory\n");
		return -1;
	}
	t1 = gettickcount();
	bmm_flush_user(src, size, BMM_DMA_TO_DEVICE);
	t2 = gettickcount();
	printf("bmm_flush_user 0x%x memory cost %dusec\n", size, t2-t1);
	free(src);

	src = bmm_malloc(size, BMM_ATTR_DEFAULT);
	if (src == NULL) {
		printf("\tno enough memory\n");
		return -1;
	}
	t1 = gettickcount();
	bmm_flush_cache_range(src, size, BMM_DMA_TO_DEVICE);
	t2 = gettickcount();
	printf("bmm_flush_cache_range 0x%x memory cost %dusec\n", size, t2-t1);
	bmm_free(src);

	return 0;
}

int test_bmm_share(unsigned long size)
{
	char *src;
	char *dst;
	char *psrc;
	char *pdst;
	unsigned long i;
	unsigned long off, len;
	unsigned long free_size1;
	unsigned long free_size2;
	unsigned long free_size3;
	int ret = 0;
	int pid = 0;
	int pid1 = 0;
	int pid2 = 0;
	int ret1 = 0;
	int ret2 = 0;

	PRINT_TEST_NAME;

	free_size1 = bmm_get_free_space();
	src = bmm_malloc(size, BMM_ATTR_DEFAULT);
	memset(src, 0, size);
	free_size2 = bmm_get_free_space();
	if(src == NULL) {
		printf("\tno enough memory\n");
		return -1;
	}
	psrc = bmm_get_paddr(src);
	printf("\tFork 3 sub processes...\n");
	pid1 = fork();
	if(pid1 != 0) {
		pid2 = fork();
		if(pid2 != 0) {
			off = 0;
			len = size;
			dst = bmm_attach(psrc + off, len);
			for(i=0; i<size; i++)
				src[i] = i;
			bmm_flush_cache(src, BMM_DMA_TO_DEVICE);
			sleep(1);
			kill(0, SIGCONT);
			waitpid(pid2, &ret2, 0);
			waitpid(pid1, &ret1, 0);
			ret2 = WEXITSTATUS(ret2) ? -1 : 0;
			ret1 = WEXITSTATUS(ret1) ? -1 : 0;
			pid = 0;
		} else {
			raise(SIGSTOP);
			off = 4096;
			len = 4096;
			dst = bmm_attach(psrc + off, len);
			pid = 1;
		}
	} else {
		pid2 = fork();
		if(pid2 != 0) {
			raise(SIGSTOP);
			off = size / 4;
			len = size / 2;
			dst = bmm_attach(psrc + off, len);
			waitpid(pid2, &ret2, 0);
			ret2 = WEXITSTATUS(ret2) ? -1 : 0;
			pid = 2;
		} else {
			raise(SIGSTOP);
			off = size - 4096;
			len = 4096;
			dst = bmm_attach(psrc + off, len);
			pid = 3;
		}
	}

	pdst = bmm_get_paddr(dst);
	if(pdst == psrc + off)
		printf("\t[%d] pdst(%p) == psrc(%p) + off(%lx)\n", pid, pdst, psrc, off);
	else
		printf("\t[%d] Error: pdst(%p) != psrc(%p) + off(%lx)\n", pid, pdst, psrc, off);

	bmm_flush_cache(dst, BMM_DMA_FROM_DEVICE);
	for(i=0; i<len; i++) {
		if(dst[i] != (char)(off+i)) {
			ret--;
			if(debug)
				printf("\t[%d] [%ld+%ld]: %d != %d\n", pid, off, i, dst[i], (char)(off+i));
			else
				break;
		}
	}

	bmm_detach(dst, len);

	printf("\t[%d] bmm_share(%ldKB/%ld-%ld) = %d\n", pid, size/1024, off, len, ret);

	if(pid1 == 0 || pid2 == 0)
		exit(ret);

	bmm_free(src);
	free_size3 = bmm_get_free_space();

	printf("\t[%d] bmm_get_free_space() return %ldKB %ldKB %ldKB\n", 
		pid, free_size1/1024, free_size2/1024, free_size3/1024);

	if(ret || ret1 || ret2)
		return ret + ret1 + ret2;

	printf("\tShare content OK. Checking free space...\n");

	return (free_size1 - free_size2 != size || free_size1 != free_size3);
}

int main(int argc, char * argv[])
{
	int ret;
	unsigned long i, n;
	pid_t pid = getpid();

	if(argc > 1) {
		debug = 1;
		if(!strcmp(argv[1], "dump")) {
			ret = test_bmm_get_space();
			PRINT_TEST_RESULT(ret);
			bmm_dump();
			return 0;
		}
	}

	printf("%s running with pid=%d/%x\n", argv[0], pid, pid);

	if(debug)
		bmm_dump();

	ret = test_bmm_init();
	PRINT_TEST_RESULT(ret);

	ret = test_bmm_get_space();
	PRINT_TEST_RESULT(ret);

	n = 8;
	for(i=1; i<=n; i++) {
		ret = test_bmm_malloc(i*1024*1024, BMM_ATTR_DEFAULT);
		PRINT_TEST_RESULT(ret);
	}
	for(i=n; i>=1; i--) {
		ret = test_bmm_malloc(i*1024*1024, BMM_ATTR_NONCACHED);
		PRINT_TEST_RESULT(ret);
	}

	n = 8;
	for(i=1; i<=n; i++) {
		ret = test_bmm_share(i*1024*1024);
		PRINT_TEST_RESULT(ret);
	}

#if 0
	n = 128;
	for(i=1; i<=n; i++) {
		ret = test_bmm_dma_memcpy(i*4096);
		PRINT_TEST_RESULT(ret);
	}

	n = 4;
	for(i=1; i<=n; i++) {
		ret = test_bmm_dma_memcpy(i*1024*1024);
		PRINT_TEST_RESULT(ret);
	}

	n = 128;
	for(i=1; i<=n; i++) {
		ret = test_bmm_dma_memcpy_range(i*4096);
		PRINT_TEST_RESULT(ret);
	}
#endif
	n = 1;
	ret = test_bmm_flush_user(n*1024*1024);
	PRINT_TEST_RESULT(ret);
	PRINT_TEST_RESULTS();

	if(debug)
		bmm_dump();

	bmm_exit();

	if(debug)
		bmm_dump();

	return 0;
}

