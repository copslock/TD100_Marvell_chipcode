/*
 * linux/sound/soc/pxa/pxa910-platform.c
 *
 * Copyright (C) 2011 Marvell International Ltd.
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

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include <mach/hardware.h>

#include "pxa3xx-pcm.h"
#include "pxa910-squ.h"
#include "pxa3xx-ssp.h"

extern struct snd_soc_platform pxa910_squ_soc_platform;
extern struct snd_soc_platform pxa3xx_soc_platform;

struct snd_soc_platform *pxa910_mix_platforms[] = {
	[SNDRV_PCM_STREAM_PLAYBACK] = &pxa910_squ_soc_platform,
	[SNDRV_PCM_STREAM_CAPTURE] = &pxa3xx_soc_platform,
};

static int pxa910_soc_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	return pxa910_mix_platforms[substream->stream]->pcm_ops->hw_params(substream, params);
}

static int pxa910_soc_hw_free(struct snd_pcm_substream *substream)
{
	return pxa910_mix_platforms[substream->stream]->pcm_ops->hw_free(substream);
}

static int pxa910_soc_prepare(struct snd_pcm_substream *substream)
{
	return pxa910_mix_platforms[substream->stream]->pcm_ops->prepare(substream);
}

static int pxa910_soc_trigger(struct snd_pcm_substream *substream, int cmd)
{
	return pxa910_mix_platforms[substream->stream]->pcm_ops->trigger(substream, cmd);
}

static snd_pcm_uframes_t
pxa910_soc_pointer(struct snd_pcm_substream *substream)
{
	return pxa910_mix_platforms[substream->stream]->pcm_ops->pointer(substream);
}

static int pxa910_soc_open(struct snd_pcm_substream *substream)
{
	return pxa910_mix_platforms[substream->stream]->pcm_ops->open(substream);
}

static int pxa910_soc_close(struct snd_pcm_substream *substream)
{
	return pxa910_mix_platforms[substream->stream]->pcm_ops->close(substream);
}

static int pxa910_soc_mmap(struct snd_pcm_substream *substream,
	struct vm_area_struct *vma)
{
	return pxa910_mix_platforms[substream->stream]->pcm_ops->mmap(substream, vma);
}

struct snd_pcm_ops pxa910_soc_ops = {
	.open		= pxa910_soc_open,
	.close		= pxa910_soc_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= pxa910_soc_hw_params,
	.hw_free	= pxa910_soc_hw_free,
	.prepare	= pxa910_soc_prepare,
	.trigger	= pxa910_soc_trigger,
	.pointer	= pxa910_soc_pointer,
	.mmap		= pxa910_soc_mmap,
};

int pxa910_soc_new(struct snd_card *card, struct snd_soc_dai *dai,
	struct snd_pcm *pcm)
{
	if (pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream) {
		pxa910_mix_platforms[SNDRV_PCM_STREAM_PLAYBACK]->pcm_new(card, dai, pcm);
	}

	if (pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream) {
		pxa910_mix_platforms[SNDRV_PCM_STREAM_CAPTURE]->pcm_new(card, dai, pcm);
	}

	return 0;
}

void pxa910_soc_free(struct snd_pcm *pcm)
{
	if (pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream)
		pxa910_mix_platforms[SNDRV_PCM_STREAM_PLAYBACK]->pcm_free(pcm);

	if (pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream)
		pxa910_mix_platforms[SNDRV_PCM_STREAM_CAPTURE]->pcm_free(pcm);

	return;
}

struct snd_soc_platform pxa910_soc_platform = {
	.name		= "pxa910-audio",
	.pcm_ops	= &pxa910_soc_ops,
	.pcm_new	= pxa910_soc_new,
	.pcm_free	= pxa910_soc_free,
};
EXPORT_SYMBOL_GPL(pxa910_soc_platform);

static int __init pxa910_soc_modinit(void)
{
	int ret = 0;

	pr_debug("[audio]-->pxa910_soc_modinit: register ssp platform driver to asoc core.\n");
	ret = snd_soc_register_platform(&pxa910_soc_platform);

	return ret;
}
module_init(pxa910_soc_modinit);

static void __exit pxa910_soc_modexit(void)
{
	snd_soc_unregister_platform(&pxa910_soc_platform);
}
module_exit(pxa910_soc_modexit);


MODULE_AUTHOR("zhouqiao@marvell.com");
MODULE_DESCRIPTION("PXA95x PCM DMA module");
MODULE_LICENSE("GPL");
