/*
 *  linux/arch/arm/mach-pxa/generic.c
 *
 *  Author:	Nicolas Pitre
 *  Created:	Jun 15, 2001
 *  Copyright:	MontaVista Software Inc.
 *
 * Code common to all PXA machines.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Since this file should be linked before any other machine specific file,
 * the __initcall() here will be executed first.  This serves as default
 * initialization stuff for PXA machines which can be overridden later if
 * need be.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/onenand.h>

#include <mach/hardware.h>
#include <asm/system.h>
#include <asm/pgtable.h>
#include <asm/mach/map.h>
#include <asm/mach-types.h>

#include <mach/reset.h>
#include <mach/gpio.h>
#include <mach/part_table.h>

#include <plat/pxa3xx_onenand.h>
#include <plat/pxa3xx_nand.h>
#include "generic.h"

#include <mach/audio.h>
void (*abu_mfp_init_func)(bool);

/* chip id is introduced from PXA95x */
unsigned int pxa_chip_id;
EXPORT_SYMBOL(pxa_chip_id);

void clear_reset_status(unsigned int mask)
{
	if (cpu_is_pxa2xx())
		pxa2xx_clear_reset_status(mask);
	else if (cpu_is_pxa3xx())
		pxa3xx_clear_reset_status(mask);
	else if (cpu_is_pxa93x())
		pxa93x_clear_reset_status(mask);
	else
		pxa95x_clear_reset_status(mask);
}

unsigned long get_clock_tick_rate(void)
{
	unsigned long clock_tick_rate;

	if (cpu_is_pxa25x())
		clock_tick_rate = 3686400;
	else if (machine_is_mainstone())
		clock_tick_rate = 3249600;
	else
		clock_tick_rate = 3250000;

	return clock_tick_rate;
}
EXPORT_SYMBOL(get_clock_tick_rate);

/*
 * Get the clock frequency as reflected by CCCR and the turbo flag.
 * We assume these values have been applied via a fcs.
 * If info is not 0 we also display the current settings.
 */
unsigned int get_clk_frequency_khz(int info)
{
	if (cpu_is_pxa25x())
		return pxa25x_get_clk_frequency_khz(info);
	else if (cpu_is_pxa27x())
		return pxa27x_get_clk_frequency_khz(info);
	return 0;
}
EXPORT_SYMBOL(get_clk_frequency_khz);

/*
 * Return the current memory clock frequency in units of 10kHz
 * Only pxa2xx_pcmcia device needs this API.
 */
unsigned int get_memclk_frequency_10khz(void)
{
	if (cpu_is_pxa25x())
		return pxa25x_get_memclk_frequency_10khz();
	else if (cpu_is_pxa27x())
		return pxa27x_get_memclk_frequency_10khz();
	return 0;
}
EXPORT_SYMBOL(get_memclk_frequency_10khz);

/*
 * Intel PXA2xx internal register mapping.
 *
 * Note 1: not all PXA2xx variants implement all those addresses.
 *
 * Note 2: virtual 0xfffe0000-0xffffffff is reserved for the vector table
 *         and cache flush area.
 *
 * Note 3: virtual 0xfb000000-0xfb00ffff is reserved for PXA95x
 *
 */
static struct map_desc standard_io_desc[] __initdata = {
  	{	/* Devs */
		.virtual	=  0xf2000000,
		.pfn		= __phys_to_pfn(0x40000000),
		.length		= 0x02000000,
		.type		= MT_DEVICE
	}, {	/* Mem Ctl */
		.virtual	=  0xf6000000,
		.pfn		= __phys_to_pfn(0x48000000),
		.length		= 0x00200000,
		.type		= MT_DEVICE
	}, {	/* Camera */
		.virtual	=  0xfa000000,
		.pfn		= __phys_to_pfn(0x50000000),
		.length		= 0x00100000,
		.type		= MT_DEVICE
	}, {	/* Sys */
		.virtual	= 0xfb000000,
		.pfn		= __phys_to_pfn(0x46000000),
		.length		= 0x00010000,
		.type		= MT_DEVICE,
	}, {	/* IMem ctl */
		.virtual	=  0xfe000000,
		.pfn		= __phys_to_pfn(0x58000000),
		.length		= 0x00100000,
		.type		= MT_DEVICE
	}, {	/* UNCACHED_PHYS_0 */
		.virtual	= 0xff000000,
		.pfn		= __phys_to_pfn(0x00000000),
		.length		= 0x00100000,
		.type		= MT_DEVICE
	}
};

#if defined(CONFIG_MTD_NAND)
static struct pxa3xx_nand_platform_data nand_info = {
	.controller_attrs = PXA3XX_ARBI_EN | PXA3XX_NAKED_CMD_EN | PXA3XX_DMA_EN | PXA3XX_ADV_TIME_TUNING | PXA3XX_POLLING_MODE,
	.parts[0] = android_512m_4k_page_partitions,
	.nr_parts[0] =   ARRAY_SIZE(android_512m_4k_page_partitions),
};


void nand_init(void)
{
	pxa3xx_set_nand_info(&nand_info);
}
#else
void nand_init(void) {}
#endif /* CONFIG_MTD_NAND */


#if (defined(CONFIG_MTD_ONENAND) || defined(CONFIG_MTD_ONENAND_MODULE))

extern void onenand_mmcontrol_smc_cfg(void);
extern void onenand_sync_clk_cfg(void);

static void __attribute__ ((unused)) onenand_mmcontrol(struct mtd_info *mtd, int sync_read)
{
        struct onenand_chip *this = mtd->priv;
        unsigned int syscfg;

        if (sync_read) {
		onenand_mmcontrol_smc_cfg();
		syscfg = this->read_word(this->base + ONENAND_REG_SYS_CFG1);
		syscfg &= (~(0x07<<9));
		/* 16 words for one burst */
		syscfg |= 0x03<<9;
		this->write_word((syscfg | sync_read ), this->base + ONENAND_REG_SYS_CFG1);
	} else {
		syscfg = this->read_word(this->base + ONENAND_REG_SYS_CFG1);
		this->write_word((syscfg & ~sync_read),this->base + ONENAND_REG_SYS_CFG1);
	}
}

static struct pxa3xx_onenand_platform_data tavorevb3_onenand_info;
static int set_partition_info(u32 flash_size, u32 page_size, struct pxa3xx_onenand_platform_data *pdata)
{
	int found = -EINVAL;
	if (256 == flash_size) {
		pdata->parts = android_256m_4k_page_partitions;
		pdata->nr_parts = ARRAY_SIZE(android_256m_4k_page_partitions);
		found = 0;
	}
	else if (512 == flash_size) {
		pdata->parts = android_512m_4k_page_partitions;
		pdata->nr_parts = ARRAY_SIZE(android_512m_4k_page_partitions);
		found = 0;
	}

	if (0 != found){
		printk(KERN_ERR"***************no proper partition table *************\n");
	}
	return found;
}
void onenand_init(int sync_enable)
{
	u32 temp;
	unsigned char __iomem *ACCR_REG_membase;
	if(sync_enable){
		onenand_sync_clk_cfg();
#define ACCR_REG 0x41340000
		ACCR_REG_membase = ioremap(ACCR_REG, 4);
		if(ACCR_REG_membase){
			temp  = ioread32(ACCR_REG_membase);
			/*
			* bit25~bit23
			* 000 78Mhz, 010 104Mhz, 100 156Mhz
			*/
		temp &= (~(7 << 23));
		iowrite32(temp | 0x02<<23, ACCR_REG_membase);  //106Mhz
		iounmap(ACCR_REG_membase);
		tavorevb3_onenand_info.mmcontrol = onenand_mmcontrol;
		}else
			printk(KERN_ERR "ERROR*****************ACCR_REG membasse ioremap failed**********************\n");
	}
	tavorevb3_onenand_info.set_partition_info = set_partition_info;
	pxa3xx_set_onenand_info(&tavorevb3_onenand_info);
}
#else
void onenand_init(int sync_enable){}
#endif

/* Board ID based on BOAR= cmdline token get from OBM */
static long g_board_id = -1;
static int __init set_board_id(char *p)
{
	int ret;
	ret = strict_strtol(p, 16, &g_board_id);
	if (ret < 0) {
		printk(KERN_ERR "%s g_board_id is not right\n", __func__);
		return ret;
	}
	printk(KERN_INFO "%s g_board_id = %ld\n", __func__, g_board_id);
	return 1;
}
__setup("BOAR=", set_board_id);

long get_board_id(void)
{
	return g_board_id;
}
EXPORT_SYMBOL(get_board_id);

void __init pxa_map_io(void)
{
	iotable_init(standard_io_desc, ARRAY_SIZE(standard_io_desc));
	get_clk_frequency_khz(1);

	if (!cpu_is_pxa2xx() || !cpu_is_pxa3xx() || !cpu_is_pxa93x())
		pxa_chip_id = __raw_readl(0xfb00ff80);
}

void __init set_abu_init_func(void (*func)(bool))
{
	if (func)
		abu_mfp_init_func = func;
	else
		printk(KERN_ERR "%s called with NULL pointer\n", __func__);
}

void pxa95x_abu_mfp_init(bool abu)
{
	if (abu_mfp_init_func)
		abu_mfp_init_func(abu);
	else
		panic("pxa95x_abu_mfp_init called with NULL pointer!\n");
}
