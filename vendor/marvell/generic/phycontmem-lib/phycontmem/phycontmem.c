/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */

#include <pthread.h>
#include "pmem_helper_lib.h"
#include "phycontmem.h"

//for bmm like
typedef struct phycontmem_node{
	struct pmem_handle_mrvl* pmem;
	struct phycontmem_node* next;
}PHYCONTMEM_NODE;

static PHYCONTMEM_NODE* g_phycontmemlist = NULL;

static pthread_mutex_t g_phycontmemlist_mutex = PTHREAD_MUTEX_INITIALIZER;

static __inline void list_add_node(PHYCONTMEM_NODE* pnode)
{
	pnode->next = g_phycontmemlist;
	g_phycontmemlist = pnode;
	return;
}

static PHYCONTMEM_NODE* list_find_by_va_range(void* VA)
{
	PHYCONTMEM_NODE* pnode;

	pnode = g_phycontmemlist;
	while(pnode) {
		if((unsigned int)VA >= (unsigned int)pnode->pmem->va && (unsigned int)VA < (unsigned int)pnode->pmem->va + pnode->pmem->size) {
			return pnode;
		}
		pnode = pnode->next;
	}
	return NULL;
}


static PHYCONTMEM_NODE* list_find_by_pa_range(unsigned int PA)
{
	PHYCONTMEM_NODE* pnode;

	pnode = g_phycontmemlist;
	while(pnode) {
		if(PA >= (unsigned int)pnode->pmem->pa && PA < (unsigned int)pnode->pmem->pa + (unsigned int)pnode->pmem->size) {
			return pnode;
		}
		pnode = pnode->next;
	}
	return NULL;
}

static PHYCONTMEM_NODE* list_remove_by_va(void* VA)
{
	PHYCONTMEM_NODE* pPrev = NULL;
	PHYCONTMEM_NODE* pnode;

	pnode = g_phycontmemlist;
	while(pnode) {
		if(VA == pnode->pmem->va) {
			break;
		}
		pPrev = pnode;
		pnode = pnode->next;
	}
	if(pnode != NULL) {
		if(pPrev) {
			pPrev->next = pnode->next;
		}else{
			g_phycontmemlist = pnode->next;
		}
	}
	return pnode;
}

void* phy_cont_malloc(int size, int attr)
{
	PHYCONTMEM_NODE* pnode;
	void* VA = NULL;
	const char* devname;
	int ret;

	if(attr == PHY_CONT_MEM_ATTR_DEFAULT) {
		devname = MARVELL_PMEMDEV_NAME_CACHEBUFFERED;
	}else if (attr == PHY_CONT_MEM_ATTR_NONCACHED) {
		devname = MARVELL_PMEMDEV_NAME_NONCACHED;
	}else if (attr == PHY_CONT_MEM_ATTR_WC) {
		devname = MARVELL_PMEMDEV_NAME_WC;
	}else{
		return NULL;
	}

	pnode = (PHYCONTMEM_NODE*)malloc(sizeof(PHYCONTMEM_NODE));
	if(pnode == NULL) {
		return NULL;
	}

	pnode->pmem = pmem_malloc(size, devname);
	if(pnode->pmem == NULL) {
		free(pnode);
		return NULL;
	}
	VA = pnode->pmem->va;

	pthread_mutex_lock(&g_phycontmemlist_mutex);
	list_add_node(pnode);
	pthread_mutex_unlock(&g_phycontmemlist_mutex);

	return VA;
}

void phy_cont_free(void* VA)
{
	PHYCONTMEM_NODE* pnode;
	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_remove_by_va(VA);
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	if(pnode) {
		pmem_free(pnode->pmem);
		free(pnode);
	}
	return;
}

unsigned int phy_cont_getpa(void* VA)
{
	PHYCONTMEM_NODE* pnode;
	unsigned int PA = 0;
	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_find_by_va_range(VA);
	if(pnode) {
		PA = ((unsigned int)VA - (unsigned int)pnode->pmem->va) + (unsigned int)pnode->pmem->pa;
	}
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	return PA;
}


void* phy_cont_getva(unsigned int PA)
{
	PHYCONTMEM_NODE* pnode;
	void* VA = NULL;
	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_find_by_pa_range(PA);
	if(pnode) {
		VA = (void*)((PA-(unsigned int)pnode->pmem->pa) + (unsigned int)pnode->pmem->va);
	}
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	return VA;
}

void phy_cont_flush_cache(void* VA, int dir)
{
	PHYCONTMEM_NODE* pnode;
	int pmem_dir;
	if(dir == PHY_CONT_MEM_FLUSH_BIDIRECTION) {
		pmem_dir = PMEM_FLUSH_BIDIRECTION;
	}else if(dir == PHY_CONT_MEM_FLUSH_TO_DEVICE) {
		pmem_dir = PMEM_FLUSH_TO_DEVICE;
	}else if(dir == PHY_CONT_MEM_FLUSH_FROM_DEVICE) {
		pmem_dir = PMEM_FLUSH_FROM_DEVICE;
	}else{
		return;
	}

	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_find_by_va_range(VA);
	if (pnode == NULL) {
		pthread_mutex_unlock(&g_phycontmemlist_mutex);
		return;
	}
	pmem_flush_cache(pnode->pmem->fd, 0, pnode->pmem->size, pmem_dir);
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	return;
}

void phy_cont_flush_cache_range(void* VA, unsigned long size, int dir)
{
	PHYCONTMEM_NODE* pnode;
	int pmem_dir;
	if(dir == PHY_CONT_MEM_FLUSH_BIDIRECTION) {
		pmem_dir = PMEM_FLUSH_BIDIRECTION;
	}else if(dir == PHY_CONT_MEM_FLUSH_TO_DEVICE) {
		pmem_dir = PMEM_FLUSH_TO_DEVICE;
	}else if(dir == PHY_CONT_MEM_FLUSH_FROM_DEVICE) {
		pmem_dir = PMEM_FLUSH_FROM_DEVICE;
	}else{
		return;
	}


	pthread_mutex_lock(&g_phycontmemlist_mutex);
	pnode = list_find_by_va_range(VA);
	if (pnode == NULL) {
		pthread_mutex_unlock(&g_phycontmemlist_mutex);
		return;
	}
	pmem_flush_cache(pnode->pmem->fd, ((unsigned long)VA-(unsigned long)pnode->pmem->va), size, pmem_dir);
	pthread_mutex_unlock(&g_phycontmemlist_mutex);
	return;
}
