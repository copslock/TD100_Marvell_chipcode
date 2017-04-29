/*
 * (C) Copyright 2011 Marvell International Ltd.
 * All Rights Reserved
 */

#ifndef __CAM_OSALBMM_H__
#define __CAM_OSALBMM_H__

#include "phycontmem.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define OSALBMM_ATTR_DEFAULT                    PHY_CONT_MEM_ATTR_DEFAULT
#define OSALBMM_ATTR_NONCACHED                  PHY_CONT_MEM_ATTR_NONCACHED

#define OSALBMM_DMA_BIDIRECTION                 PHY_CONT_MEM_FLUSH_BIDIRECTION
#define OSALBMM_DMA_TO_DEVICE                   PHY_CONT_MEM_FLUSH_TO_DEVICE
#define OSALBMM_DMA_FROM_DEVICE                 PHY_CONT_MEM_FLUSH_FROM_DEVICE

#define osalbmm_malloc                          phy_cont_malloc
#define osalbmm_free                            phy_cont_free
#define osalbmm_get_paddr                       phy_cont_getpa
#define osalbmm_get_vaddr                       phy_cont_getva

#define osalbmm_flush_cache                     phy_cont_flush_cache
#define osalbmm_flush_cache_range               phy_cont_flush_cache_range


#ifdef __cplusplus
}
#endif

#endif	// __CAM_OSALBMM_H__
