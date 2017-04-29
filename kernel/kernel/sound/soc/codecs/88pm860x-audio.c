/*
 * linux/sound/soc/codecs/88pm860x_audio.c
 * Base on linux/sound/soc/codecs/wm8753.c
 *
 * Copyright (C) 2007 Marvell International Ltd.
 * Author: Bin Yang <bin.yang@marvell.com>
 *			 Yael Sheli Chemla<yael.s.shemla@marvell.com>
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
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/clk.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <linux/mfd/88pm860x.h>
#include <asm/io.h>
#include <linux/delay.h>         /* for msleep / usleep definition */

#include "88pm860x-audio.h"

#define A_DEBUG_MUST  KERN_ALERT

#define AUDIO_NAME "pm860x audio codec"
#define PM860X_AUDIO_VERSION "0.4"

#define ARRAY_AND_SIZE(x)	x, ARRAY_SIZE(x)

#define MAX_NAME_LEN		20

/* codec private data */
struct pm860x_priv {
	unsigned int		sysclk;
	unsigned int		pcmclk;
	unsigned int		dir;
	unsigned int		filter;
	unsigned int		playback_samplerate;
	unsigned int		capture_samplerate;
	struct snd_soc_codec	*codec;
	struct i2c_client	*i2c;
	struct pm860x_chip	*chip;
	struct clk		*clk;
};

static struct snd_soc_codec *pm860x_codec;

static const struct snd_kcontrol_new levante_direct_access[] = {
	/* MIsc Register for audio clock configuration */
	SOC_SINGLE("LEVANTE_MISC1", MISC1, 0, 0xff, 0),                                 	/* 0x40 */
	SOC_SINGLE("LEVANTE_MCLK", MCLK, 0, 0xff, 0),                                   	/* 0x41 */
	SOC_SINGLE("LEVANTE_MISC2", MISC2, 0, 0xff, 0),						/* 0x42 */

	/* Audio Register */
	SOC_SINGLE("LEVANTE_PLL_CTRL1", PLL_CTRL1, 0, 0xff, 0),					/* 0x43 */
	SOC_SINGLE("LEVANTE_PLL_FRAC1", PLL_FRAC1, 0, 0xff, 0),					/* 0x44 */
	SOC_SINGLE("LEVANTE_PLL_FRAC2", PLL_FRAC2, 0, 0xff, 0),					/* 0x45 */
	SOC_SINGLE("LEVANTE_PLL_FRAC3", PLL_FRAC3, 0, 0xff, 0),					/* 0x46 */
	/* support dynamically switch sample rate between 44.1k and 8k */
	SOC_SINGLE("LEVANTE_SAMPLE_RATE_SWITCH", SAMPLE_RATE_SWITCH, 0, 0xff, 0),		/* 0x47 */

	SOC_SINGLE("LEVANTE_PCM_INTERFACE1", PCM_INTERFACE1, 0, 0xff, 0),			/* 0xB0 */
	SOC_SINGLE("LEVANTE_PCM_INTERFACE2", PCM_INTERFACE2, 0, 0xff, 0),			/* 0xB1 */
	SOC_SINGLE("LEVANTE_PCM_INTERFACE3", PCM_INTERFACE3, 0, 0xff, 0),			/* 0xB2 */
	SOC_SINGLE("LEVANTE_PCM_RATE", ADC_PCM, 0, 0xff, 0),					/* 0xB3 */
	SOC_SINGLE("LEVANTE_ECHO_CANCEL_PATH", ECHO_CANCEL_PATH, 0, 0xff, 0),			/* 0xB4 */
	SOC_SINGLE("LEVANTE_SIDETONE_GAIN1", SIDETONE_GAIN1, 0, 0xff, 0),			/* 0xB5 */
	SOC_SINGLE("LEVANTE_SIDETONE_GAIN2", SIDETONE_GAIN2, 0, 0xff, 0),			/* 0xB6 */
	SOC_SINGLE("LEVANTE_SIDETONE_GAIN3", SIDETONE_GAIN3, 0, 0xff, 0),			/* 0xB7 */
	SOC_SINGLE("LEVANTE_ADC_OFFSET1", ADC_OFFSET1, 0, 0xff, 0),				/* 0xB8 */
	SOC_SINGLE("LEVANTE_ADC_OFFSET2", ADC_OFFSET2, 0, 0xff, 0),				/* 0xB9 */
	SOC_SINGLE("LEVANTE_DMIC_DELAY", DMIC_DELAY, 0, 0xff, 0),				/* 0xBA */
	SOC_SINGLE("LEVANTE_I2S_INTERFACE1", I2S_INTERFACE_1, 0, 0xff, 0),			/* 0xBB */
	SOC_SINGLE("LEVANTE_I2S_INTERFACE2", I2S_INTERFACE_2, 0, 0xff, 0),			/* 0xBC */
	SOC_SINGLE("LEVANTE_I2S_INTERFACE3", I2S_INTERFACE_3, 0, 0xff, 0),			/* 0xBD */
	SOC_SINGLE("LEVANTE_I2S_INTERFACE4", I2S_INTERFACE_4, 0, 0xff, 0),			/* 0xBE */
	SOC_SINGLE("LEVANTE_EQUALIZER_N0_1", EQUALIZER_N0_1, 0, 0xff, 0),			/* 0xBF */
	SOC_SINGLE("LEVANTE_EQUALIZER_N0_2", EQUALIZER_N0_2, 0, 0xff, 0),			/* 0xC0 */
	SOC_SINGLE("LEVANTE_EQUALIZER_N1_1", EQUALIZER_N1_1, 0, 0xff, 0),			/* 0xC1 */
	SOC_SINGLE("LEVANTE_EQUALIZER_N1_2", EQUALIZER_N1_2, 0, 0xff, 0),			/* 0xC2 */
	SOC_SINGLE("LEVANTE_EQUALIZER_D1_1", EQUALIZER_D1_1, 0, 0xff, 0),			/* 0xC3 */
	SOC_SINGLE("LEVANTE_EQUALIZER_D1_2", EQUALIZER_D1_2, 0, 0xff, 0),			/* 0xC4 */
	SOC_SINGLE("LEVANTE_SIDE_TONE1", SIDE_TONE_1, 0, 0xff, 0),				/* 0xC5 */
	SOC_SINGLE("LEVANTE_SIDE_TONE2", SIDE_TONE_2, 0, 0xff, 0),				/* 0xC6 */
	SOC_SINGLE("LEVANTE_LEFT_GAIN1", LEFT_GAIN1, 0, 0xff, 0),				/* 0xC7 */
	SOC_SINGLE("LEVANTE_LEFT_GAIN2", LEFT_GAIN2, 0, 0xff, 0),				/* 0xC8 */
	SOC_SINGLE("LEVANTE_RIGHT_GAIN1", RIGHT_GAIN1, 0, 0xff, 0),				/* 0xC9 */
	SOC_SINGLE("LEVANTE_RIGHT_GAIN2", RIGHT_GAIN2, 0, 0xff, 0),				/* 0xCA */
	SOC_SINGLE("LEVANTE_DAC_OFFSET", DAC_OFFSET, 0, 0xff, 0),				/* 0xCB */
	SOC_SINGLE("LEVANTE_OFFSET_LEFT1", OFFSET_LEFT1, 0, 0xff, 0),				/* 0xCC */
	SOC_SINGLE("LEVANTE_OFFSET_LEFT2", OFFSET_LEFT2, 0, 0xff, 0),				/* 0xCD */
	SOC_SINGLE("LEVANTE_OFFSET_RIGHT1", OFFSET_RIGHT1, 0, 0xff, 0),				/* 0xCE */
	SOC_SINGLE("LEVANTE_OFFSET_RIGHT2", OFFSET_RIGHT2, 0, 0xff, 0),				/* 0xCF */
	SOC_SINGLE("LEVANTE_ADC_ANALOG_PROGRAM1", ADC_ANALOG_PROGRAM1, 0, 0xff, 0),		/* 0xD0 */
	SOC_SINGLE("LEVANTE_ADC_ANALOG_PROGRAM2", ADC_ANALOG_PROGRAM2, 0, 0xff, 0),		/* 0xD1 */
	SOC_SINGLE("LEVANTE_ADC_ANALOG_PROGRAM3", ADC_ANALOG_PROGRAM3, 0, 0xff, 0),		/* 0xD2 */
	SOC_SINGLE("LEVANTE_ADC_ANALOG_PROGRAM4", ADC_ANALOG_PROGRAM4, 0, 0xff, 0),		/* 0xD3 */
	SOC_SINGLE("LEVANTE_ANALOG_TO_ANALOG", ANALOG_TO_ANALOG, 0, 0xff, 0),			/* 0xD4 */
	SOC_SINGLE("LEVANTE_HS1_CTRL", HS1_CTRL, 0, 0xff, 0),					/* 0xD5 */
	SOC_SINGLE("LEVANTE_HS2_CTRL", HS2_CTRL, 0, 0xff, 0),					/* 0xD6 */
	SOC_SINGLE("LEVANTE_LO1_CTRL", LO1_CTRL, 0, 0xff, 0),					/* 0xD7 */
	SOC_SINGLE("LEVANTE_LO2_CTRL", LO2_CTRL, 0, 0xff, 0),					/* 0xD8 */
	SOC_SINGLE("LEVANTE_EAR_SPKR_CTRL1", EAR_SPKR_CTRL1, 0, 0xff, 0),			/* 0xD9 */
	SOC_SINGLE("LEVANTE_EAR_SPKR_CTRL2", EAR_SPKR_CTRL2, 0, 0xff, 0),			/* 0xDA */
	SOC_SINGLE("LEVANTE_AUDIO_SUPPLIES1", AUDIO_SUPPLIES1, 0, 0xff, 0),			/* 0xDB */
	SOC_SINGLE("LEVANTE_AUDIO_SUPPLIES2", AUDIO_SUPPLIES2, 0, 0xff, 0),			/* 0xDC */
	SOC_SINGLE("LEVANTE_ADC_ENABLES1", ADC_ENABLES1, 0, 0xff, 0),				/* 0xDD */
	SOC_SINGLE("LEVANTE_ADC_ENABLES2", ADC_ENABLES2, 0, 0xff, 0),				/* 0xDE */
	SOC_SINGLE("LEVANTE_DAC_ENABLES1", DAC_ENABLES1, 0, 0xff, 0),				/* 0xDF */
	SOC_SINGLE("LEVANTE_DAC_DUMMY", DUMMY, 0, 0xff, 0),					/* 0xE0 */
	SOC_SINGLE("LEVANTE_DAC_ENABLES2", DAC_ENABLES2, 0, 0xff, 0),				/* 0xE1 */
	SOC_SINGLE("LEVANTE_AUDIO_CAL1", AUDIO_CAL1, 0, 0xff, 0),				/* 0xE2 */
	SOC_SINGLE("LEVANTE_AUDIO_CAL2", AUDIO_CAL2, 0, 0xff, 0),				/* 0xE3 */
	SOC_SINGLE("LEVANTE_AUDIO_CAL3", AUDIO_CAL3, 0, 0xff, 0),				/* 0xE4 */
	SOC_SINGLE("LEVANTE_AUDIO_CAL4", AUDIO_CAL4, 0, 0xff, 0),				/* 0xE5 */
	SOC_SINGLE("LEVANTE_AUDIO_CAL5", AUDIO_CAL5, 0, 0xff, 0),				/* 0xE6 */
	SOC_SINGLE("LEVANTE_ANALOG_INPUT_SEL1", ANALOG_INPUT_SEL1, 0, 0xff, 0),			/* 0xE7 */
	SOC_SINGLE("LEVANTE_ANALOG_INPUT_SEL2", ANALOG_INPUT_SEL2, 0, 0xff, 0),			/* 0xE8 */
	SOC_SINGLE("LEVANTE_MIC_BUTTON_DETECTION", PCM_INTERFACE4, 0, 0xff, 0),			/* 0xE9 */
	SOC_SINGLE("LEVANTE_HEADSET_DETECTION", I2S_INTERFACE_5, 0, 0xff, 0),			/* 0xEA */
	SOC_SINGLE("LEVANTE_SHORTS", SHORTS, 0, 0xff, 0),					/* 0xEB */
};

#define LEVANTE_ID_V  (67)

static int levante_id_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	unsigned char value;
	struct snd_soc_codec *codec = kcontrol->private_data;

	if (codec == NULL) {
		pr_debug("invalid param\n");
		return -EINVAL;
	}

	value = pm860x_reg_read(codec->control_data, 0x00);

	ucontrol->value.integer.value[0] = value;
	return 0;
}


static const struct snd_kcontrol_new levante_id_read =
	SOC_SINGLE_EXT("LEVANTE_ID", LEVANTE_ID_V, 0, 0xff, 0, levante_id_get, NULL);


static int levante_add_direct_access(struct snd_soc_codec *codec)
{
	int err, i;
	for (i = 0; i < ARRAY_SIZE(levante_direct_access); i++) {
		err = snd_ctl_add(codec->card,
			snd_soc_cnew(&levante_direct_access[i], codec, NULL));
		if (err < 0)
			return err;
	}
	return 0;
}

static int levante_id_access(struct snd_soc_codec *codec)
{
	int err;
	err = snd_ctl_add(codec->card, snd_soc_cnew(&levante_id_read, codec, NULL));
	if (err < 0)
		return err;

	return 0;
}


/*
 * read pm860x audio register cache
 */
static unsigned int pm860x_audio_read(struct snd_soc_codec *codec, unsigned int reg)
{
	u8 *cache = codec->reg_cache;

	if (reg > (ARRAY_SIZE(levante_direct_access)))
		return -EIO;

	return cache[reg];
}


/*
 * write to the pm860x audio register space
 */
static int pm860x_audio_write(struct snd_soc_codec *codec, unsigned int reg,
	unsigned int value)
{
	int offset;
	u8 *cache = codec->reg_cache;
	struct pm860x_priv *pm860x_audio = codec->drvdata;

	if (reg == SAMPLE_RATE_SWITCH) {
		/** Sample Rate Mapping
		  * 0x0 - 5512HZ
		  * 0x1 - 8000HZ
		  * 0x2 - 11025HZ
		  * 0x3 - 16000HZ
		  * 0x4 - 22050HZ
		  * 0x5 - 32000HZ
		  * 0x6 - 44100HZ
		  * 0x7 - 48000HZ
		  * 0x8 - 64000HZ
		  * 0x9 - 88200HZ
		  * 0xa - 96000HZ
		  * 0xb - 176400HZ
		  * 0xc - 192000HZ
		 **/
		pm860x_audio->playback_samplerate = 1 << (value & 0xf);
		pm860x_audio->capture_samplerate = 1 << ((value >> 4) & 0xf);

		cache[reg] = value;
		return 0;
	}

	if (reg < PM860X_AUDIO_GROUP_B_OFFSET_START)
		offset = PM860X_AUDIO_GROUP_A_FIRST_REG;
	else if (reg <= PM860X_AUDIO_GROUP_B_OFFSET_END)
		offset = PM860X_AUDIO_GROUP_B_FIRST_REG - PM860X_AUDIO_GROUP_B_OFFSET_START;
	else
		return -EIO;

	if (reg == MISC2) { /* handle LEVANTE_MISC2 (0x42) register */
		if ((value & ((1 << 3) | (1 << 4))) == ((1 << 3) | (1 << 4))) {
			pm860x_reg_write(codec->control_data, reg + offset, (1 << 3));
		/* add 160 uSec delay between writing bit 3 and bit 4 (with
		 * all the others) */
			udelay(160);
		}
	}

	pm860x_reg_write(codec->control_data, reg + offset, value);

	cache[reg] = value;

	return 0;
}


/*
 * pm860x audio codec reset
 */
static int pm860x_audio_reset(struct snd_soc_codec *codec)
{
	unsigned int reg, offset;
	u8 *cache = codec->reg_cache;

	for (reg = 0; reg < ARRAY_SIZE(levante_direct_access); reg++) {
		if (reg == SAMPLE_RATE_SWITCH) {
			cache[reg] = 0x0;
			continue;
		}

		if (reg < PM860X_AUDIO_GROUP_B_OFFSET_START)
			offset = PM860X_AUDIO_GROUP_A_FIRST_REG;
		else
			offset = PM860X_AUDIO_GROUP_B_FIRST_REG -
				PM860X_AUDIO_GROUP_B_OFFSET_START;

		cache[reg] = pm860x_reg_read(codec->control_data, reg + offset);
	}

	return 0;
}


/*
 * add non dapm controls
 */
static int pm860x_audio_add_controls(struct snd_soc_codec *codec)
{
	levante_add_direct_access(codec);
	levante_id_access(codec);
	return 0;
}


/*
 * add dapm controls
 */
static int pm860x_audio_add_widgets(struct snd_soc_codec *codec)
{
	return 0;
}


/* set HIFI DAI configuration */
static int pm860x_aduio_hifi_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
	return 0;
}

static int pm860x_audio_hifi_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params, struct snd_soc_dai *dai __maybe_unused)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev->card->codec;

	int rate, value;
	rate =  params_rate(params);
	value = pm860x_audio_read(codec, 0x0e);
	value &= 0xf0;
	switch(rate){
		case 48000:
			value |= 0x08;
			break;
		case 44100:
			value |= 0x07;
			break;
		case 32000:
			value |= 0x06;
			break;
		case 24000:
			value |= 0x05;
			break;
		case 22050:
			value |= 0x04;
			break;
		case 16000:
			value |= 0x03;
			break;
		case 12000:
			value |= 0x02;
			break;
		case 11025:
			value |= 0x01;
			break;
		case 8000:
			value |= 0x00;
			break;
		default:
			pr_debug("unsupported rate\n");
			return -EINVAL;
			break;
	}
	/*pm860x_audio_write(codec, 0x0e, value); pm860x has sample rate switching issue*/

	return 0;
}

static int pm860x_audio_mute(struct snd_soc_dai *dai, int mute)
{
	return 0;
}

static int pm860x_audio_set_bias_level(struct snd_soc_codec *codec,  enum snd_soc_bias_level level)
{
	switch (level) {
	        case SND_SOC_BIAS_ON: /* full On */
			break;

		case SND_SOC_BIAS_PREPARE: /* partial On */
			break;

		case SND_SOC_BIAS_STANDBY: /* partial On */
			break;

		case SND_SOC_BIAS_OFF: /* Off, without power */
			break;
	}

	codec->bias_level = level;

	return 0;
}

#define PM860X_AUDIO_HIFI_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |\
		                SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |\
				SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000)
#define PM860X_AUDIO_VOICE_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 | \
				SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_48000)
#define PM860X_AUDIO_HIFI_FORMATS (SNDRV_PCM_FMTBIT_S16_LE)
#define PM860X_AUDIO_VOICE_FORMATS (SNDRV_PCM_FMTBIT_S16_LE)

/* initiate playback and capture sample rate to pm860x audio sample_rate */
static int pm860x_audio_startup(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	struct pm860x_priv *pm860x_audio = codec->drvdata;

	dai->playback.rates = pm860x_audio->playback_samplerate;
	dai->capture.rates = pm860x_audio->capture_samplerate;

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		if ((dai->capture.rates & PM860X_AUDIO_VOICE_RATES) == 0) {
			printk("unsupported catpure rate: 0x%x\n", dai->capture.rates);
			return -1;
		}
	}

	return 0;
}

static struct snd_soc_dai_ops pm860x_dai_ops = {
	.startup = pm860x_audio_startup,
	.hw_params = pm860x_audio_hifi_hw_params,
	.digital_mute = pm860x_audio_mute,
	.set_fmt = pm860x_aduio_hifi_set_dai_fmt,
	.set_clkdiv = NULL,
	.set_pll = NULL,
	.set_sysclk = NULL,
};

/*
 * HIFI DAI
 */
struct snd_soc_dai pm860x_audio_dai[]={
/* DAI HIFI mode*/
	{
		.name = "pm860x audio HiFi",
		.id = 0,
		.playback = {
			.stream_name = "HiFi Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = PM860X_AUDIO_HIFI_RATES,
			.formats = PM860X_AUDIO_HIFI_FORMATS,
		},
		.capture = {
			.stream_name = "HiFi Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = PM860X_AUDIO_HIFI_RATES,
			.formats = PM860X_AUDIO_HIFI_FORMATS,
		},
		.ops = &pm860x_dai_ops,
	},

	{
		.name = "pm860x audio pcm",
		.id = 1,
		.playback = {
			.stream_name = "Pcm Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = PM860X_AUDIO_VOICE_RATES,
			.formats = PM860X_AUDIO_VOICE_FORMATS,
		},
		.capture = {
			.stream_name = "Pcm Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = PM860X_AUDIO_VOICE_RATES,
			.formats = PM860X_AUDIO_VOICE_FORMATS,
		},
		.ops = &pm860x_dai_ops,
	},

	{
		.name = "pm860x audio HiFi playback only",
		.id = 2,
		.playback = {
			.stream_name = "HiFi Playback only",
			.channels_min = 1,
			.channels_max = 2,
			.rates = PM860X_AUDIO_HIFI_RATES,
			.formats = PM860X_AUDIO_HIFI_FORMATS,
		},
		.ops = &pm860x_dai_ops,
	},

	{
		.name = "pm860x audio pcm capture only",
		.id = 3,
		.capture = {
			.stream_name = "HiFi Capture only",
			.channels_min = 1,
			.channels_max = 2,
			.rates = PM860X_AUDIO_VOICE_RATES,
			.formats = PM860X_AUDIO_VOICE_FORMATS,
		},
		.ops = &pm860x_dai_ops,
	},
};

static void pm860x_audio_work(struct work_struct *work)
{
	struct snd_soc_codec *codec =
			container_of(work, struct snd_soc_codec, delayed_work.work);
	pm860x_audio_set_bias_level(codec, codec->bias_level);
}


static int pm860x_audio_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;
	struct pm860x_priv *pm860x_audio = codec->drvdata;
	u8 *cache = codec->reg_cache;

	/*make sure that we are not using PCM interface or AUX input,
	  consider the case if we suspend during a voice call or FM radio*/
	if (((cache[ADC_ENABLES2] & (1 << 0)) == 0) &&
		((cache[ADC_ENABLES1] & (1 << 4)) == 0)) {
		/* write these two registers to reset all the audio registers,
			need to observe whether will get pop and click
			And should write audio regitster before audio section
			is turned off */
		pm860x_reg_write(codec->control_data,
			(AUDIO_SUPPLIES2 - PM860X_AUDIO_GROUP_B_OFFSET_START +
			PM860X_AUDIO_GROUP_B_FIRST_REG), 0x00);
		pm860x_reg_write(codec->control_data,
			(MISC2 + PM860X_AUDIO_GROUP_A_FIRST_REG), 0x10);
		pm860x_reg_write(codec->control_data,
			(MISC2 + PM860X_AUDIO_GROUP_A_FIRST_REG), 0x00);
		if (pm860x_audio->clk)
			clk_disable(pm860x_audio->clk);
	}

	return 0;
}


static int pm860x_audio_resume(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;
	struct pm860x_priv *pm860x_audio = codec->drvdata;
	u8 *cache = codec->reg_cache;
	int i;

	if (((cache[ADC_ENABLES2] & (1 << 0)) == 0) && pm860x_audio->clk &&
			((cache[ADC_ENABLES1] & (1 << 4)) == 0))
		clk_enable(pm860x_audio->clk);

	/* if no audio path need to be enabled or we did not really
		uspend since we are in a voice call */
	if ((cache[MISC2] & (1 << 5)) && (cache[MISC2] & (1 << 3)) &&
			((cache[ADC_ENABLES2] & (1 << 0)) == 0)) {
		/* set some registers first, to avoid pop and click */
		/* in order to trun on audio section, AUDIO_PU,
			AUDIO_RSTB and PLL_PU should be set to 1 */
		pm860x_reg_write(codec->control_data,
			(MISC2 + PM860X_AUDIO_GROUP_A_FIRST_REG), cache[MISC2]);

		/* The audio I2C registers (register addresses 0x0 to 0xEE) are
			essible for read or write activity 160 usec after
			AUDIO_PU set to '1' */
		msleep(1);

		/* enable LDO15, enable audio charge pump, audio is active */
		pm860x_reg_write(codec->control_data,
			(AUDIO_SUPPLIES2 - PM860X_AUDIO_GROUP_B_OFFSET_START +
			PM860X_AUDIO_GROUP_B_FIRST_REG),
			cache[AUDIO_SUPPLIES2]);

		if (cache[DAC_ENABLES2] & (1 << 3)) {
			/*Enable 0xbc*/
			pm860x_reg_write(codec->control_data,
					 (I2S_INTERFACE_2 -
					  PM860X_AUDIO_GROUP_B_OFFSET_START +
					  PM860X_AUDIO_GROUP_B_FIRST_REG),
					 cache[I2S_INTERFACE_2]);
			/*Enable 0xbe*/
			pm860x_reg_write(codec->control_data,
					 (I2S_INTERFACE_4 -
					  PM860X_AUDIO_GROUP_B_OFFSET_START +
					  PM860X_AUDIO_GROUP_B_FIRST_REG),
					 cache[I2S_INTERFACE_4]);
			/* Enable DAC always for avoid noise */
			pm860x_reg_write(codec->control_data,
					 (DAC_ENABLES2 -
					  PM860X_AUDIO_GROUP_B_OFFSET_START +
					  PM860X_AUDIO_GROUP_B_FIRST_REG),
					 cache[DAC_ENABLES2]);
		}

		/* apply changes */
		pm860x_reg_write(codec->control_data,
			(EAR_SPKR_CTRL2 - PM860X_AUDIO_GROUP_B_OFFSET_START +
			PM860X_AUDIO_GROUP_B_FIRST_REG),
			cache[EAR_SPKR_CTRL2] & (~(1 << 2)));
		pm860x_reg_write(codec->control_data,
			(EAR_SPKR_CTRL2 - PM860X_AUDIO_GROUP_B_OFFSET_START +
			PM860X_AUDIO_GROUP_B_FIRST_REG),
			cache[EAR_SPKR_CTRL2] | (1 << 2));

		/* restore the registers */
		for (i = PM860X_AUDIO_GROUP_B_OFFSET_START;
			i <= PM860X_AUDIO_GROUP_B_OFFSET_END; i++) {
			if (!
			    ((i == AUDIO_SUPPLIES2) || (i == I2S_INTERFACE_4)
			     || (i == DAC_ENABLES2) || (i == I2S_INTERFACE_2)))
				pm860x_reg_write(codec->control_data,
					(i - PM860X_AUDIO_GROUP_B_OFFSET_START +
					PM860X_AUDIO_GROUP_B_FIRST_REG),
					cache[i]);
		}

		/* apply changes */
		pm860x_reg_write(codec->control_data,
			(EAR_SPKR_CTRL2 - PM860X_AUDIO_GROUP_B_OFFSET_START +
			PM860X_AUDIO_GROUP_B_FIRST_REG),
			cache[EAR_SPKR_CTRL2] & (~(1 << 2)));
		pm860x_reg_write(codec->control_data,
			(EAR_SPKR_CTRL2 - PM860X_AUDIO_GROUP_B_OFFSET_START +
			PM860X_AUDIO_GROUP_B_FIRST_REG),
			cache[EAR_SPKR_CTRL2] | (1 << 2));
	}

	return 0;
}

/* check audio short status: enable HS1/2 amplifier if detecting
 * audio short interrupt */
static irqreturn_t pm860x_short_handler(int irq, void *data)
{
	struct pm860x_priv *pm860x = data;
	int dac_status, hs_status;

	/* check audio SHORTS status */
	hs_status = pm860x_reg_read(pm860x->i2c, PM8607_AUDIO_REG_BASE
					+ PM8607_AUDIO_SHORTS);
	pr_debug("%s: Headset short status 0x%x\n", __func__, hs_status);
	/* check whether HS1 SHORT status is set */
	if (hs_status & 0x02) {
		/* clear HS1 status */
		pm860x_set_bits(pm860x->i2c, PM8607_AUDIO_REG_BASE
					+ PM8607_AUDIO_SHORTS, 0x02, 0x02);
		/* check DAC reg */
		dac_status = pm860x_reg_read(pm860x->i2c,
					PM8607_AUDIO_REG_BASE + DAC_ENABLES1);
		if (!(dac_status & 0x1)) {
			/* HS1 amplifier has been disabled, enable it */
			pm860x_set_bits(pm860x->i2c, PM8607_AUDIO_REG_BASE
					+ DAC_ENABLES1, 0x1, 1);
		}
	}

	/* check whether HS2 SHORT status is set */
	if (hs_status & 0x08) {
		/* clear HS2 status */
		pm860x_set_bits(pm860x->i2c, PM8607_AUDIO_REG_BASE
					+ PM8607_AUDIO_SHORTS, 0x08, 0x08);
		/* check DAC reg */
		dac_status = pm860x_reg_read(pm860x->i2c,
					PM8607_AUDIO_REG_BASE + DAC_ENABLES1);
		if (!(dac_status & 0x2)) {
			/* HS2 amplifier has been disabled, enable it */
			pm860x_set_bits(pm860x->i2c, PM8607_AUDIO_REG_BASE
					+ DAC_ENABLES1, 0x2, 0x2);
		}
	}

	pr_debug("%s warning: if this mesg appears too frequent"
			"please check sanremo hardware\n", __func__);

	return IRQ_HANDLED;
}

static int pm860x_audio_init(struct snd_soc_device *socdev)
{
	struct snd_soc_codec *codec = socdev->card->codec;
	int ret = 0;

	/* register pcms */
	ret = snd_soc_new_pcms(socdev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1);
	if (ret < 0) {
		pr_debug("pm860x audio: failed to create pcms\n");
		goto pcm_err;
	}

	codec->bias_level = SND_SOC_BIAS_STANDBY;
	schedule_delayed_work(&codec->delayed_work,msecs_to_jiffies(2));

	pm860x_audio_reset(codec);

	pm860x_audio_add_controls(codec);
	pm860x_audio_add_widgets(codec);

	return ret;

pcm_err:
	return ret;
}

static int pm860x_audio_probe(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec;
	struct pm860x_priv *pm860x_audio;
	int ret = 0;

	if (pm860x_codec == NULL) {
		dev_err(&pdev->dev, "Codec device not registered\n");
		return -ENODEV;
		}

	pr_debug("pm860x Audio Codec %s\n", PM860X_AUDIO_VERSION);

	socdev->card->codec = pm860x_codec;
	codec = pm860x_codec;

	pm860x_audio = snd_soc_codec_get_drvdata(codec);
	if (!pm860x_audio) {
		dev_err(&pdev->dev, "Codec drv data invalid\n");
		return -EINVAL;
	}

	/* initiate pm860x audio sample rate to default sample rate */
	pm860x_audio->playback_samplerate = SNDRV_PCM_RATE_44100;
	pm860x_audio->capture_samplerate = SNDRV_PCM_RATE_8000;

	mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);
	INIT_DELAYED_WORK(&codec->delayed_work, pm860x_audio_work);

	pm860x_audio_init(socdev);

	return ret;
}


/*
 * This function forces any delayed work to be queued and run.
 */
static int run_delayed_work(struct delayed_work *dwork)
{
	int ret;

	/* cancel any work waiting to be queued. */
	ret = cancel_delayed_work(dwork);

	/* if there was any work waiting then we run it now and
	 * wait for it's completion */
	if (ret) {
		schedule_delayed_work(dwork, 0);
		flush_scheduled_work();
	}
	return ret;
}

/* power down chip */
static int pm860x_audio_remove(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	if (codec->control_data)
		pm860x_audio_set_bias_level(codec, SND_SOC_BIAS_OFF);

	run_delayed_work(&codec->delayed_work);
	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);

	kfree(codec->drvdata);
	kfree(codec);

	return 0;
}

struct snd_soc_codec_device soc_codec_dev_pm860x_audio = {
	.probe =	pm860x_audio_probe,
	.remove =	pm860x_audio_remove,
	.suspend =	pm860x_audio_suspend,
	.resume =	pm860x_audio_resume,
};

EXPORT_SYMBOL_GPL(soc_codec_dev_pm860x_audio);

static int __devinit pm860x_codec_probe(struct platform_device *pdev)
{
	struct pm860x_chip *chip = dev_get_drvdata(pdev->dev.parent);
	struct pm860x_priv *pm860x;
	struct snd_soc_codec *codec;
	struct resource *res;
	unsigned int short_irq;
	int i, ret;

	if (pm860x_codec) {
		dev_err(&pdev->dev, "Another pm860x is registered\n");
		return -EINVAL;
		}

	pm860x = kzalloc(sizeof(struct pm860x_priv), GFP_KERNEL);
	if (pm860x == NULL)
		return -ENOMEM;

	pm860x->codec = kzalloc(sizeof(struct snd_soc_codec), GFP_KERNEL);
	if (pm860x->codec == NULL)
	{
		kfree(pm860x);
		return -ENOMEM;
	}

	pm860x->chip = chip;
	pm860x->i2c = (chip->id == CHIP_PM8607) ? chip->client
			: chip->companion;
	platform_set_drvdata(pdev, pm860x);

	pm860x->clk = clk_get(NULL, "VCTCXO");
	if (IS_ERR(pm860x->clk)) {
		pr_err("unable to get VCTCXO\n");
		pm860x->clk = NULL;
	}
	if (pm860x->clk)
		clk_enable(pm860x->clk);

	/* get audio short irq */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
	if (res) {
		short_irq = res->start + chip->irq_base;
		ret = request_threaded_irq(short_irq, NULL,
				pm860x_short_handler, IRQF_ONESHOT,
				"audio short", pm860x);
		if (ret < 0)
			dev_err(&pdev->dev, "Failed to request IRQ: #%d: %d\n",
					short_irq, ret);
	} else
		pr_info("[88pm860x-audio]: short irq not defined\n");

	codec = pm860x->codec;

	mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);

	snd_soc_codec_set_drvdata(codec, pm860x);

	codec->name = "pm860x audio";
	codec->owner = THIS_MODULE;
	codec->read = pm860x_audio_read;
	codec->write = pm860x_audio_write;
	codec->set_bias_level = pm860x_audio_set_bias_level;
	codec->dai = pm860x_audio_dai;
	codec->num_dai = ARRAY_SIZE(pm860x_audio_dai);
	codec->reg_cache_size = ARRAY_SIZE(levante_direct_access);
	codec->reg_cache = kzalloc(codec->reg_cache_size, GFP_KERNEL);
	if (codec->reg_cache == NULL) {
		ret = -ENOMEM;
		goto err_irq;
	}
	codec->control_data = pm860x->i2c;
	codec->dev = &pdev->dev;

	pm860x_codec = codec;

	for (i = 0; i < ARRAY_SIZE(pm860x_audio_dai); i++)
			pm860x_audio_dai[i].dev = codec->dev;

	ret = snd_soc_register_codec(codec);
	if (ret != 0) {
		dev_err(codec->dev, "Failed to register codec: %d\n", ret);
		goto err_cache;
	}

	ret = snd_soc_register_dais(pm860x_audio_dai, ARRAY_SIZE(pm860x_audio_dai));
	if (ret != 0) {
		dev_err(codec->dev, "Failed to register DAIs: %d\n", ret);
		goto err_codec;
	}

	return ret;

err_codec:
	snd_soc_unregister_codec(codec);
err_cache:
	kfree(codec->reg_cache);
err_irq:
	platform_set_drvdata(pdev, NULL);
	kfree(pm860x->codec);
	kfree(pm860x);
	return ret;
}

static int __devexit pm860x_codec_remove(struct platform_device *pdev)
{
	struct pm860x_priv *pm860x = platform_get_drvdata(pdev);

	snd_soc_unregister_dais(pm860x_audio_dai, ARRAY_SIZE(pm860x_audio_dai));
	snd_soc_unregister_codec(pm860x->codec);

	kfree(pm860x->codec->reg_cache);

	platform_set_drvdata(pdev, NULL);
	kfree(pm860x->codec);
	kfree(pm860x);
	pm860x_codec = NULL;
	return 0;
}

static struct platform_driver pm860x_codec_driver = {
	.driver	= {
		.name	= "88pm860x-codec",
		.owner	= THIS_MODULE,
	},
	.probe	= pm860x_codec_probe,
	.remove	= __devexit_p(pm860x_codec_remove),
};

static int __init pm860x_audio_modinit(void)
{
	return platform_driver_register(&pm860x_codec_driver);
}
module_init(pm860x_audio_modinit);

static void __exit pm860x_audio_exit(void)
{
	platform_driver_unregister(&pm860x_codec_driver);
}
module_exit(pm860x_audio_exit);

MODULE_DESCRIPTION("ASoC pm860x audio driver");
MODULE_AUTHOR("xjian@marvell.com");
MODULE_LICENSE("GPL");
