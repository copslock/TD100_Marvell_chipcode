/*
 *  linux/drivers/mtd/onenand/pxa3xx_onenand.c
 *
 *  Copyright (C) 2009 Marvell Corporation
 *  Haojian Zhuang <haojian.zhuang@marvell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/onenand.h>
#include <linux/mtd/partitions.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/dma.h>
#include <asm/mach/flash.h>
#include <mach/hardware.h>
#include <mach/dma.h>
#include <asm/cacheflush.h>
#include <mach/dvfm.h>
#include <plat/pxa3xx_onenand.h>

#ifdef CONFIG_PXA3XX_BBM
#include <plat/pxa3xx_bbm.h>
#endif

#define DRIVER_NAME	"pxa3xx-onenand"

#define SEQUENCE_TWO

#ifdef CONFIG_MTD_PARTITIONS
static const char *part_probes[] = { "cmdlinepart", NULL,  };
#endif

static struct dvfm_lock dvfm_lock = {
	.lock		= SPIN_LOCK_UNLOCKED,
	.dev_idx	= -1,
	.count		= 0,
};

static void set_dvfm_constraint(void)
{
	spin_lock_irqsave(&dvfm_lock.lock, dvfm_lock.flags);
	if (dvfm_lock.count++ == 0) {
		/* Disable Low power mode */
		dvfm_disable_op_name("D1", dvfm_lock.dev_idx);
		dvfm_disable_op_name("D2", dvfm_lock.dev_idx);
		dvfm_disable_op_name("CG", dvfm_lock.dev_idx);
	}
	spin_unlock_irqrestore(&dvfm_lock.lock, dvfm_lock.flags);
}

static void unset_dvfm_constraint(void)
{
	spin_lock_irqsave(&dvfm_lock.lock, dvfm_lock.flags);
	if (dvfm_lock.count == 0) {
		spin_unlock_irqrestore(&dvfm_lock.lock, dvfm_lock.flags);
		return;
	}
	if (--dvfm_lock.count == 0) {
		/* Enable Low power mode */
		dvfm_enable_op_name("D1", dvfm_lock.dev_idx);
		dvfm_enable_op_name("D2", dvfm_lock.dev_idx);
		dvfm_enable_op_name("CG", dvfm_lock.dev_idx);
	}
	spin_unlock_irqrestore(&dvfm_lock.lock, dvfm_lock.flags);
}

void pxa3xx_onenand_get_device(struct mtd_info *mtd)
{
	struct pxa3xx_onenand_info *info =
			container_of(mtd, struct pxa3xx_onenand_info, mtd);
	clk_enable(info->smc_clk);
	set_dvfm_constraint();
}

void pxa3xx_onenand_release_device(struct mtd_info *mtd)
{
	struct pxa3xx_onenand_info *info =
			container_of(mtd, struct pxa3xx_onenand_info, mtd);
	unset_dvfm_constraint();
	clk_disable(info->smc_clk);
}
EXPORT_SYMBOL(pxa3xx_onenand_get_device);
EXPORT_SYMBOL(pxa3xx_onenand_release_device);

static void dma_nodesc_handler(int irq, void *data)
{
	struct pxa3xx_onenand_info *info = (struct pxa3xx_onenand_info *)data;
	int i;

	DCSR(irq) = DCSR_STARTINTR|DCSR_ENDINTR|DCSR_BUSERR;

	for (i = 0; i < info->channel_cnt; i++) {
		if (info->dma_ch[i] == irq)
			info->dma_ch_done[i] = 1;
	}
	for (i = 0; i < info->channel_cnt; i++) {
		if (info->dma_ch_done[i] == 0) {
			return;
		}
	}
	complete(&info->dma_done);
	return;
}

static inline int pxa3xx_onenand_bufferram_offset(struct mtd_info *mtd, int area)
{
	struct onenand_chip *this = mtd->priv;

	if (ONENAND_CURRENT_BUFFERRAM(this)) {
		if (area == ONENAND_DATARAM)
			return mtd->writesize;
		if (area == ONENAND_SPARERAM)
			return mtd->oobsize;
	}
	return 0;
}

/**
 * pxa3xx_onenand_read_bufferram - [OneNAND Interface] Read the bufferram area
 * 			with Sync. Burst mode
 * @param mtd		MTD data structure
 * @param area		BufferRAM area
 * @param buffer	the databuffer to put/get data
 * @param offset	offset to read from or write to
 * @param count		number of bytes to read/write
 *
 * Read the BufferRAM area with Sync. Burst Mode
 */
static int pxa3xx_onenand_read_bufferram(struct mtd_info *mtd, int area,
		unsigned char *buffer, int offset, size_t count)
{
	struct pxa3xx_onenand_info *info =
			container_of(mtd, struct pxa3xx_onenand_info, mtd);
	struct onenand_chip *this = &info->onenand;
	int bram_offset, len, off, i;
	dma_addr_t dma_src, dma_dst;
	void *buf = (void *)buffer;
	int sum, orig_offset = offset;
	u32 onenand_ram_size;

	bram_offset = pxa3xx_onenand_bufferram_offset(mtd, area) + area + offset;
	onenand_ram_size = ONENAND_IS_4KB_PAGE(this) ? ONENAND_RAM_SIZE * 2 : ONENAND_RAM_SIZE;
	if (offset & (onenand_ram_size - 1)) {
		/* make offset aligned */
		offset = ALIGN(offset - onenand_ram_size, onenand_ram_size);
	}
	len = ALIGN((count + orig_offset - offset) / MAX_DMA_CHANNEL - 31, 32);
	dma_dst = info->sram_addr_phys + offset;
	dma_src = info->phys_base + area + offset + pxa3xx_onenand_bufferram_offset(mtd, area);

	this->mmcontrol(mtd, ONENAND_SYS_CFG1_SYNC_READ);
	for (i = 0; i < MAX_DMA_CHANNEL; i++)
		info->dma_ch_done[i] = 0;
	INIT_COMPLETION(info->dma_done);

	if (MAX_DMA_CHANNEL > 1) {
		/* use multiple channel */
		sum = count + orig_offset - offset;
		len = ALIGN(sum / MAX_DMA_CHANNEL - 31, 32);
		for (i = 0, off = 0; i < MAX_DMA_CHANNEL; i++, off += len) {
			/* update remaing count */
			if (i == (MAX_DMA_CHANNEL - 1))
				len = sum;
			sum = sum - len;
			DCSR(info->dma_ch[i]) = DCSR_NODESC;
			DSADR(info->dma_ch[i]) = dma_src + off;
			DTADR(info->dma_ch[i]) = dma_dst + off;
			DCMD(info->dma_ch[i]) = DCMD_INCSRCADDR | DCMD_INCTRGADDR
				| DCMD_BURST32 | DCMD_WIDTH2
				| DCMD_ENDIRQEN | len;
			DCSR(info->dma_ch[i]) |= DCSR_RUN;
		}
		info->channel_cnt = MAX_DMA_CHANNEL;
	} else {
		/* use single channel */
		DCSR(info->dma_ch[0]) = DCSR_NODESC;
		DSADR(info->dma_ch[0]) = dma_src;
		DTADR(info->dma_ch[0]) = dma_dst;
		DCMD(info->dma_ch[0]) = DCMD_INCSRCADDR | DCMD_INCTRGADDR
					| DCMD_BURST32 | DCMD_WIDTH2
					| DCMD_ENDIRQEN
					| (count + orig_offset - offset);
		DCSR(info->dma_ch[0]) |= DCSR_RUN;
		info->channel_cnt = 1;
	}
	wait_for_completion(&info->dma_done);
	memcpy(buf, info->sram_addr + orig_offset - offset, count);

	this->mmcontrol(mtd, 0);
	return 0;
}

static int pxa3xx_onenand_bbt(struct mtd_info *mtd)
{
	int ret;
	struct pxa3xx_onenand_info *info =
			container_of(mtd, struct pxa3xx_onenand_info, mtd);

	 ret = pxa3xx_scan_bbt(mtd);
	 if (ret){
		 printk(KERN_INFO "pxa3xx_scan_bbt  failed\n");
		 goto out;
	 }
	 info->bbm = mtd->bbm;

out:
	return onenand_default_bbt(mtd);
}

static int __devinit pxa3xx_onenand_probe(struct platform_device *dev)
{
	struct pxa3xx_onenand_info *info;
	struct platform_device *pdev = dev;
	struct pxa3xx_onenand_platform_data *pdata = pdev->dev.platform_data;
	struct resource *res = pdev->resource;
	struct onenand_chip *this = NULL;
	unsigned long size = res->end - res->start + 1;
	int err = 0, i, ret;
	char name[80];

	info = kzalloc(sizeof(struct pxa3xx_onenand_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	this = &info->onenand;
	init_completion(&info->irq_done);
	init_completion(&info->dma_done);

	if (!request_mem_region(res->start, size, pdev->name)) {
		err = -EBUSY;
		goto out_free_info;
	}

	info->phys_base = res->start;
	info->onenand.base = ioremap(res->start, size);
	if (!info->onenand.base) {
		err = -ENOMEM;
		goto out_release_mem_region;
	}

	size = ONENAND_REG_MANUFACTURER_ID - ONENAND_DATARAM;
	info->onenand.data = ioremap_cached(res->start + ONENAND_DATARAM, size);
	if (!info->onenand.data) {
		err = -ENOMEM;
		goto out_iounmap;
	}

	if (pdata->mmcontrol) {
		/*
		 * If synchronous mode is to be used, mmcontrol() should be
		 * defined in platform driver.
		 */
		info->onenand.mmcontrol = pdata->mmcontrol;
		info->onenand.read_bufferram = pxa3xx_onenand_read_bufferram;
	}
#ifdef CONFIG_PXA3XX_BBM
	info->onenand.scan_bbt = pxa3xx_onenand_bbt;
	info->onenand.block_markbad = pxa3xx_block_markbad;
	info->onenand.block_bad = pxa3xx_block_bad;
#endif

	info->onenand.irq = platform_get_irq(pdev, 0);
	info->mtd.name = dev_name(&pdev->dev);

	info->mtd.priv = &info->onenand;
	info->mtd.owner = THIS_MODULE;

	dvfm_register("ONENAND", &dvfm_lock.dev_idx);

	info->smc_clk = clk_get(NULL, "SMCCLK");
	clk_enable(info->smc_clk);

	for (i = 0; i < MAX_DMA_CHANNEL; i++)
		info->dma_ch[i] = -1;	/* not allocated */

	for (i = 0; i < MAX_DMA_CHANNEL; i++) {
		sprintf(name, "ONENAND_%d", i);
		info->dma_ch[i] = pxa_request_dma(name, DMA_PRIO_HIGH,
						dma_nodesc_handler,
						(void *)info);
		if (info->dma_ch[i] < 0) {
			ret = -ENODEV;
			goto out_freedma;
		}
	}

	info->sram_addr = dma_alloc_coherent(NULL, 4096,
				&info->sram_addr_phys, GFP_KERNEL);
	if (onenand_scan(&info->mtd, 1)) {
		err = -ENXIO;
		goto out_freedma;
	}

	/* remount mmcontrol in order to enable private read_bufferram */
	if (pdata->mmcontrol)
		info->onenand.read_bufferram = pxa3xx_onenand_read_bufferram;

#ifdef CONFIG_MTD_PARTITIONS
	if(pdata->set_partition_info){
		pdata->set_partition_info((info->onenand.chipsize)>>20, info->onenand.writesize, pdata);
	}
	err = parse_mtd_partitions(&info->mtd, part_probes, &info->parts, 0);
	if (err > 0)
		add_mtd_partitions(&info->mtd, info->parts, err);
	else if (err <= 0 && pdata->parts)
		add_mtd_partitions(&info->mtd, pdata->parts, pdata->nr_parts);
	else
#endif
		err = add_mtd_device(&info->mtd);

	dev_set_drvdata(&pdev->dev, info);

	clk_disable(info->smc_clk);

	return 0;

out_freedma:
	clk_disable(info->smc_clk);

	for (i = 0; i < MAX_DMA_CHANNEL; i++) {
		if (info->dma_ch[i] >= 0)
			pxa_free_dma(info->dma_ch[i]);
	}
out_iounmap:
	iounmap(info->onenand.base);
	iounmap(info->onenand.data);
out_release_mem_region:
	release_mem_region(res->start, size);
out_free_info:
	kfree(info);

	return err;
}

static int __devexit pxa3xx_onenand_remove(struct platform_device *dev)
{
	struct platform_device *pdev = dev;
	struct pxa3xx_onenand_info *info = dev_get_drvdata(&pdev->dev);
	struct resource *res = pdev->resource;
	unsigned long size = res->end - res->start + 1;
	int i;

	dev_set_drvdata(&pdev->dev, NULL);

	if (info) {
		for (i = 0; i < MAX_DMA_CHANNEL; i++) {
			if (info->dma_ch[i] >= 0)
				pxa_free_dma(info->dma_ch[i]);
		}

		if (info->parts)
			del_mtd_partitions(&info->mtd);
		else
			del_mtd_device(&info->mtd);

		onenand_release(&info->mtd);
		if (info->bbm && info->bbm->uninit) {
			info->bbm->uninit(&info->mtd);
		}
		release_mem_region(res->start, size);
		iounmap(info->onenand.base);
		iounmap(info->onenand.data);
		kfree(info);
	}

	dvfm_unregister("ONENAND", &dvfm_lock.dev_idx);

	return 0;
}

static struct platform_driver pxa3xx_onenand_driver = {
	.driver = {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= pxa3xx_onenand_probe,
	.remove		= pxa3xx_onenand_remove,
#ifdef CONFIG_PM
	.suspend	= NULL,
	.resume		= NULL,
#endif
};

static int __init pxa3xx_onenand_init(void)
{
	return platform_driver_register(&pxa3xx_onenand_driver);
}

static void __exit pxa3xx_onenand_exit(void)
{
	platform_driver_unregister(&pxa3xx_onenand_driver);
}

module_init(pxa3xx_onenand_init);
module_exit(pxa3xx_onenand_exit);

MODULE_LICENSE("GPL");
MODULE_ALIAS(DRIVER_NAME);
MODULE_AUTHOR("Haojian Zhuang <haojian.zhuang@marvell.com>");
MODULE_DESCRIPTION("Glue layer for OneNAND flash on PXA3xx");
