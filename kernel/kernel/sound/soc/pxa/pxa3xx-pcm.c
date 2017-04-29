/*
 * linux/sound/soc/pxa/pxa3xx-pcm.c
 *
 * Base on linux/sound/soc/pxa/pxa2xx-pcm.c
 *
 * Copyright (C) 2007 Marvell International Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/wakelock.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <plat/ssp.h>

#include <asm/dma.h>

#include <mach/dma.h>
#include "pxa3xx-pcm.h"

static struct wake_lock pxa3xx_pcm_idle_lock;

static const struct snd_pcm_hardware pxa3xx_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_MMAP |
				  SNDRV_PCM_INFO_MMAP_VALID |
				  SNDRV_PCM_INFO_INTERLEAVED |
				  SNDRV_PCM_INFO_PAUSE |
				  SNDRV_PCM_INFO_RESUME,
	.formats		= SNDRV_PCM_FMTBIT_S16_LE |
					SNDRV_PCM_FMTBIT_S24_LE |
					SNDRV_PCM_FMTBIT_S32_LE,
	.period_bytes_min	= 32,
	.period_bytes_max	= 8192 - 32,
	.periods_min		= 1,
	.periods_max		= PAGE_SIZE/sizeof(pxa_dma_desc),
	.buffer_bytes_max	= 112 * 1024,
	.fifo_size		= 32,
};

struct pxa3xx_runtime_data {
	int dma_ch;
	struct pxa3xx_pcm_dma_params *params;
	void *dma_desc_array;
	dma_addr_t dma_desc_array_phys;
};



static void pxa3xx_pcm_dma_irq(int dma_ch, void *dev_id)
{
	struct snd_pcm_substream *substream = dev_id;
	int dcsr;

	dcsr = DCSR(dma_ch);
	DCSR(dma_ch) = dcsr & ~DCSR_STOPIRQEN;
	if (dcsr & DCSR_ENDINTR) {
		snd_pcm_period_elapsed(substream);
	} else {
		printk(KERN_ERR "DMA error on channel %d (DCSR=%#x)\n",
			dma_ch, dcsr);
	}
}


#define GSSP_BASE_PHY_ADDR (APB_PHYS_BASE + 0x00039000)

static int pxa3xx_pcm_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct pxa3xx_runtime_data *prtd = runtime->private_data;

	size_t totsize = params_buffer_bytes(params);
	size_t period = params_period_bytes(params);
	pxa_dma_desc *dma_desc;
	dma_addr_t dma_buff_phys, next_desc_phys;
	int ret;

	/* this may get called several times by oss emulation
	 * with different params */
	if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		ret = pxa_request_dma("GSSP PCM out", DMA_PRIO_LOW,
					pxa3xx_pcm_dma_irq, substream);
	else
		ret = pxa_request_dma("GSSP PCM in", DMA_PRIO_LOW,
					pxa3xx_pcm_dma_irq, substream);

	if (ret < 0)
		return ret;
	prtd->dma_ch = ret;

	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);
	runtime->dma_bytes = totsize;

	next_desc_phys = prtd->dma_desc_array_phys;
	dma_buff_phys = runtime->dma_addr;

	dma_desc = prtd->dma_desc_array;
	do {
		next_desc_phys += sizeof(pxa_dma_desc);

		dma_desc->ddadr = next_desc_phys;
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			dma_desc->dsadr = dma_buff_phys;
			dma_desc->dtadr = GSSP_BASE_PHY_ADDR + SSDR;
		} else {
			dma_desc->dsadr = GSSP_BASE_PHY_ADDR + SSDR;
			dma_desc->dtadr = dma_buff_phys;
		}
		if (period > totsize)
			period = totsize;

		if(substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			dma_desc->dcmd = DCMD_WIDTH4 | DCMD_BURST8 | DCMD_FLOWTRG
						| DCMD_INCSRCADDR | period | DCMD_ENDIRQEN;
		else
			dma_desc->dcmd = DCMD_WIDTH4 | DCMD_BURST8 | DCMD_FLOWSRC
						| DCMD_INCTRGADDR | period | DCMD_ENDIRQEN;

		dma_desc++;
		dma_buff_phys += period;
	} while (totsize -= period);
	dma_desc[-1].ddadr = prtd->dma_desc_array_phys;

	return 0;
}

static int pxa3xx_pcm_hw_free(struct snd_pcm_substream *substream)
{
	struct pxa3xx_runtime_data *prtd = substream->runtime->private_data;

	if (prtd->dma_ch != -1) {
		snd_pcm_set_runtime_buffer(substream, NULL);
		pxa_free_dma(prtd->dma_ch);
		prtd->dma_ch = -1;
	}

	return 0;
}

static int pxa3xx_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct pxa3xx_runtime_data *prtd = substream->runtime->private_data;

	DCSR(prtd->dma_ch) &= ~DCSR_RUN;
	DCSR(prtd->dma_ch) = 0;
	DCMD(prtd->dma_ch) = 0;
	*prtd->params->drcmr = prtd->dma_ch | DRCMR_MAPVLD;

	return 0;
}

static int pxa3xx_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct pxa3xx_runtime_data *prtd = substream->runtime->private_data;
	int ret = 0;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		wake_lock(&pxa3xx_pcm_idle_lock);
		DDADR(prtd->dma_ch) = prtd->dma_desc_array_phys;
		DCSR(prtd->dma_ch) = DCSR_RUN;
		break;

	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		DCSR(prtd->dma_ch) &= ~DCSR_RUN;
		wake_unlock(&pxa3xx_pcm_idle_lock);
		break;

	case SNDRV_PCM_TRIGGER_RESUME:
		wake_lock(&pxa3xx_pcm_idle_lock);
		DCSR(prtd->dma_ch) |= DCSR_RUN;
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		wake_lock(&pxa3xx_pcm_idle_lock);
		DDADR(prtd->dma_ch) = prtd->dma_desc_array_phys;
		DCSR(prtd->dma_ch) |= DCSR_RUN;
		break;

	default:
		ret = -EINVAL;
	}
	return ret;
}

static snd_pcm_uframes_t
pxa3xx_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct pxa3xx_runtime_data *prtd = runtime->private_data;
	dma_addr_t ptr;
	snd_pcm_uframes_t x;

	ptr = (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) ?
			DSADR(prtd->dma_ch) : DTADR(prtd->dma_ch);

	x = bytes_to_frames(runtime, ptr - runtime->dma_addr);

	if (x == runtime->buffer_size)
		x = 0;
	return x;
}

static int pxa3xx_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct pxa3xx_runtime_data *prtd;
	int ret;

	snd_soc_set_runtime_hwparams(substream, &pxa3xx_pcm_hardware);

	/*
	 * For mysterious reasons (and despite what the manual says)
	 * playback samples are lost if the DMA count is not a multiple
	 * of the DMA burst size.  Let's add a rule to enforce that.
	 */
	ret = snd_pcm_hw_constraint_step(runtime, 0,
		SNDRV_PCM_HW_PARAM_PERIOD_BYTES, 32);
	if (ret)
		goto out;

	ret = snd_pcm_hw_constraint_step(runtime, 0,
		SNDRV_PCM_HW_PARAM_BUFFER_BYTES, 32);
	if (ret)
		goto out;

	ret = snd_pcm_hw_constraint_integer(runtime,
					SNDRV_PCM_HW_PARAM_PERIODS);
	if (ret < 0)
		goto out;

	prtd = kzalloc(sizeof(struct pxa3xx_runtime_data), GFP_KERNEL);
	if (prtd == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	prtd->params = kzalloc(sizeof(struct pxa3xx_pcm_dma_params), GFP_KERNEL);
	if (prtd->params == NULL) {
		ret = -ENOMEM;
		goto err1;
	}

	/* FIXME: consider to add GSSP device later to remove hard code */
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		prtd->params->drcmr = &DRCMR(7);
	else
		prtd->params->drcmr = &DRCMR(6);

	prtd->dma_ch = -1;
	prtd->dma_desc_array =
		dma_alloc_writecombine(substream->pcm->card->dev, PAGE_SIZE,
				&prtd->dma_desc_array_phys, GFP_KERNEL);

	if (!prtd->dma_desc_array) {
		ret = -ENOMEM;
		goto err2;
	}

	runtime->private_data = prtd;
	return 0;

 err2:
	kfree(prtd->params);
 err1:
	kfree(prtd);
 out:
	return ret;
}

static int pxa3xx_pcm_close(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct pxa3xx_runtime_data *prtd = runtime->private_data;
	dma_free_writecombine(substream->pcm->card->dev, PAGE_SIZE,
			prtd->dma_desc_array, prtd->dma_desc_array_phys);
	kfree(prtd->params);
	kfree(prtd);
	return 0;
}

static int pxa3xx_pcm_mmap(struct snd_pcm_substream *substream,
	struct vm_area_struct *vma)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	return dma_mmap_writecombine(substream->pcm->card->dev, vma,
				runtime->dma_area,
				runtime->dma_addr,
				runtime->dma_bytes);
}

struct snd_pcm_ops pxa3xx_pcm_ops = {
	.open		= pxa3xx_pcm_open,
	.close		= pxa3xx_pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= pxa3xx_pcm_hw_params,
	.hw_free	= pxa3xx_pcm_hw_free,
	.prepare	= pxa3xx_pcm_prepare,
	.trigger	= pxa3xx_pcm_trigger,
	.pointer	= pxa3xx_pcm_pointer,
	.mmap		= pxa3xx_pcm_mmap,
};

static int pxa3xx_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;

	size_t size = pxa3xx_pcm_hardware.buffer_bytes_max;
	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->area = dma_alloc_writecombine(pcm->card->dev, size,
				   &buf->addr, GFP_KERNEL);
	if (!buf->area)
		return -ENOMEM;
	buf->bytes = size;
	return 0;
}

static void pxa3xx_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;
		dma_free_writecombine(pcm->card->dev, buf->bytes,
					buf->area, buf->addr);
		buf->area = NULL;
	}
}

static u64 pxa3xx_pcm_dmamask = DMA_BIT_MASK(32);

int pxa3xx_pcm_new(struct snd_card *card, struct snd_soc_dai *dai,
	struct snd_pcm *pcm)
{
	int ret = 0;

	if (!card->dev->dma_mask)
		card->dev->dma_mask = &pxa3xx_pcm_dmamask;

	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = DMA_BIT_MASK(32);

	if (dai->playback.channels_min) {
		ret = pxa3xx_pcm_preallocate_dma_buffer(pcm,SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}

	if (dai->capture.channels_min) {
		ret = pxa3xx_pcm_preallocate_dma_buffer(pcm,SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}
 out:
	return ret;
}

struct snd_soc_platform pxa3xx_soc_platform = {
	.name		= "pxa3xx-audio",
	.pcm_ops	= &pxa3xx_pcm_ops,
	.pcm_new	= pxa3xx_pcm_new,
	.pcm_free	= pxa3xx_pcm_free_dma_buffers,
};
EXPORT_SYMBOL_GPL(pxa3xx_soc_platform);

static int __init pxa3xx_soc_platform_init(void)
{
	wake_lock_init(&pxa3xx_pcm_idle_lock, WAKE_LOCK_IDLE, "pxa3xx_pcm_idle");

	return snd_soc_register_platform(&pxa3xx_soc_platform);
}
module_init(pxa3xx_soc_platform_init);

static void __exit pxa3xx_soc_platform_exit(void)
{
	wake_lock_destroy(&pxa3xx_pcm_idle_lock);
	snd_soc_unregister_platform(&pxa3xx_soc_platform);
}
module_exit(pxa3xx_soc_platform_exit);

MODULE_AUTHOR("bin.yang@marvell.com");
MODULE_DESCRIPTION("PXA3xx PCM DMA module");
MODULE_LICENSE("GPL");
