/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include <ctype.h>

#if defined(BIONIC)
#include <asoundlib.h>
#else
#include <alsa/asoundlib.h>
#endif

#include <pxa_dbg.h>

#define NUMID_OFFSET            1
#define MICCO_AUDIO_OFFSET      0x70

#define LEVANTE_AUDIO_REG_BASE1         0x40
#define LEVANTE_AUDIO_REG_END1          0x47
#define LEVANTE_AUDIO_REG_BASE2         0xB0
#define LEVANTE_AUDIO_REG_END2          0xEB

#define LEVANTE_CODEC_REG_BASE1         0x00
#define LEVANTE_CODEC_REG_END1          (LEVANTE_AUDIO_REG_END1 - LEVANTE_AUDIO_REG_BASE1)

#define LEVANTE_CODEC_REG_BASE2         (LEVANTE_CODEC_REG_END1 + 1)
#define LEVANTE_CODEC_REG_END2          (LEVANTE_AUDIO_REG_END2 - LEVANTE_AUDIO_REG_BASE2 + LEVANTE_CODEC_REG_BASE2)

#define LEVANTE_AUDIO_REGS_NUM          (LEVANTE_CODEC_REG_END2 + 1)

char card[64] = "default";


void MICCOI2CWrite(unsigned char I2CRegAddr, unsigned char I2CRegData)
{

	int err;
	snd_ctl_t *handle;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *control;
	unsigned int idx, count;

	snd_ctl_elem_info_t **pinfo = &info;
	snd_ctl_elem_id_t **pid = &id;
	snd_ctl_elem_value_t **pcontrol = &control;

	int numid = 0;
	int value = 0;

	numid = I2CRegAddr - MICCO_AUDIO_OFFSET + NUMID_OFFSET;
	value = I2CRegData;

	snd_ctl_elem_type_t type;
	snd_ctl_elem_info_alloca(pinfo);
	snd_ctl_elem_id_alloca(pid);
	snd_ctl_elem_value_alloca(pcontrol);


	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_numid(id, numid);

	if ((err = snd_ctl_open(&handle, card, 0)) < 0)
	{
		DPRINTF("snd_ctl_open error: cannot open the default card: \n");
		return;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(handle, info)) < 0)
	{
		DPRINTF("MICCOI2CWrite::snd_ctl_elem_info error1\n");
		return;
	}

	snd_ctl_elem_info_get_id(info, id);
	type = snd_ctl_elem_info_get_type(info);
	count = snd_ctl_elem_info_get_count(info);
	snd_ctl_elem_value_set_id(control, id);

	for (idx = 0; idx < count && idx < 128; idx++)
	{
		switch (type)
		{

		case SND_CTL_ELEM_TYPE_INTEGER:
			if ( (value >= snd_ctl_elem_info_get_min(info)) &&
			     (value <= snd_ctl_elem_info_get_max(info)) )
				snd_ctl_elem_value_set_integer(control, idx, value);
			break;


		case SND_CTL_ELEM_TYPE_ENUMERATED:
			if ( (value <= (int)(snd_ctl_elem_info_get_items(info) - 1)) && (value >= 0))
				snd_ctl_elem_value_set_enumerated(control, idx, value);
			break;

		default:
			DPRINTF("type not support\n");
			break;
		}
	}

	if ((err = snd_ctl_elem_write(handle, control)) < 0)
	{
		DPRINTF("snd_ctl_elem_write error\n");
		return;
	}

	snd_ctl_close(handle);
	return;
}

unsigned int MICCOI2CRead(unsigned char I2CRegAddr, unsigned char *I2CRegData)
{
	snd_hctl_t *hctl;
	snd_ctl_t *handle;
	snd_hctl_elem_t *elem;

	snd_ctl_elem_info_t *info;
	snd_ctl_elem_value_t *control;
	snd_ctl_elem_id_t *id;

	int err;
	unsigned int count, idx;
	snd_ctl_elem_type_t type;
	int numid = 0;
	int value = 0;

	snd_ctl_elem_info_t **pinfo = &info;
	snd_ctl_elem_id_t **pid = &id;
	snd_ctl_elem_value_t **pcontrol = &control;

	snd_ctl_elem_info_alloca(pinfo);
	snd_ctl_elem_value_alloca(pcontrol);
	snd_ctl_elem_id_alloca(pid);

	numid = I2CRegAddr - MICCO_AUDIO_OFFSET + NUMID_OFFSET;


	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_numid(id, numid);

	if ((err = snd_ctl_open(&handle, card, 0)) < 0)
	{
		DPRINTF("snd_ctl_open error: cannot open the default card \n");
		return err;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(handle, info)) < 0)
	{
		DPRINTF("MICCOI2CRead::snd_ctl_elem_info error2\n");
		return err;
	}

	snd_ctl_elem_info_get_id(info, id);
	type = snd_ctl_elem_info_get_type(info);
	count = snd_ctl_elem_info_get_count(info);
	snd_ctl_elem_value_set_id(control, id);

	snd_ctl_close(handle);


	if ((err = snd_hctl_open(&hctl, card, 0)) < 0)
	{
		DPRINTF("snd_hctl_open error: cannot open the default card \n");
		return err;
	}

	if ((err = snd_hctl_load(hctl)) < 0)
	{
		DPRINTF("snd_hctl_load failed\n");
		return err;
	}

	elem = snd_hctl_find_elem(hctl, id);

	if (!elem)
	{
		DPRINTF("snd_hctl_find_elem: can not find elem\n");
		return 1;
	}

	snd_ctl_elem_info_alloca(pinfo);

	if ((err = snd_hctl_elem_info(elem, info)) < 0)
	{
		DPRINTF("snd_hctl_elem_info failed\n");
		return err;
	}

	count = snd_ctl_elem_info_get_count(info);
	type = snd_ctl_elem_info_get_type(info);

	if ((err = snd_hctl_elem_read(elem, control)) < 0)
	{
		DPRINTF("snd_hctl_elem_read failed\n");
		return err;
	}

	for (idx = 0; idx < count; idx++)
	{
		switch (type)
		{

		case SND_CTL_ELEM_TYPE_INTEGER:
			value = snd_ctl_elem_value_get_integer(control, idx);
			break;


		case SND_CTL_ELEM_TYPE_ENUMERATED:
			value = snd_ctl_elem_value_get_enumerated(control, idx);
			break;

		default:
			DPRINTF("type not support\n");
			break;
		}
	}

	*I2CRegData = value;
	snd_hctl_close(hctl);
	return 0;
}

unsigned int LEVANTEI2CRead(unsigned char I2CRegAddr, unsigned char *I2CRegData)
{
	snd_hctl_t *hctl;
	snd_ctl_t *handle;
	snd_hctl_elem_t *elem;

	snd_ctl_elem_info_t *info;
	snd_ctl_elem_value_t *control;
	snd_ctl_elem_id_t *id;

	int err;
	unsigned int count, idx;
	snd_ctl_elem_type_t type;
	int numid = 0;
	int value = 0;

	snd_ctl_elem_info_t **pinfo = &info;
	snd_ctl_elem_id_t **pid = &id;
	snd_ctl_elem_value_t **pcontrol = &control;

	snd_ctl_elem_info_alloca(pinfo);
	snd_ctl_elem_value_alloca(pcontrol);
	snd_ctl_elem_id_alloca(pid);

	numid = I2CRegAddr;

	if ( (numid >= LEVANTE_AUDIO_REG_BASE1) && (numid <= LEVANTE_AUDIO_REG_END1) )
	{
		numid = numid - LEVANTE_AUDIO_REG_BASE1 + LEVANTE_CODEC_REG_BASE1;
	}
	else if ( (numid >= LEVANTE_AUDIO_REG_BASE2) && (numid <= LEVANTE_AUDIO_REG_END2) )
	{
		numid = numid - LEVANTE_AUDIO_REG_BASE2 + LEVANTE_CODEC_REG_BASE2;
	}
	else
		DPRINTF("LevanteI2CWrite; Out of range ERROR; numid= %d\n", numid);

	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_numid(id, numid);

	if ((err = snd_ctl_open(&handle, card, 0)) < 0)
	{
		DPRINTF("snd_ctl_open error: cannot open the default card \n");
		return err;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(handle, info)) < 0)
	{
		DPRINTF("LevanteI2CRead::snd_ctl_elem_info error3\n");
		return err;
	}

	snd_ctl_elem_info_get_id(info, id);
	type = snd_ctl_elem_info_get_type(info);
	count = snd_ctl_elem_info_get_count(info);
	snd_ctl_elem_value_set_id(control, id);

	snd_ctl_close(handle);


	if ((err = snd_hctl_open(&hctl, card, 0)) < 0)
	{
		DPRINTF("snd_hctl_open error: cannot open the default card \n");
		return err;
	}

	if ((err = snd_hctl_load(hctl)) < 0)
	{
		DPRINTF("snd_hctl_load failed\n");
		return err;
	}

	elem = snd_hctl_find_elem(hctl, id);

	if (!elem)
	{
		DPRINTF("snd_hctl_find_elem: can not find elem\n");
		return 1;
	}

	snd_ctl_elem_info_alloca(pinfo);

	if ((err = snd_hctl_elem_info(elem, info)) < 0)
	{
		DPRINTF("snd_hctl_elem_info failed\n");
		return err;
	}

	count = snd_ctl_elem_info_get_count(info);
	type = snd_ctl_elem_info_get_type(info);

	if ((err = snd_hctl_elem_read(elem, control)) < 0)
	{
		DPRINTF("snd_hctl_elem_read failed\n");
		return err;
	}

	for (idx = 0; idx < count; idx++)
	{
		switch (type)
		{

		case SND_CTL_ELEM_TYPE_INTEGER:
			value = snd_ctl_elem_value_get_integer(control, idx);
			break;


		case SND_CTL_ELEM_TYPE_ENUMERATED:
			value = snd_ctl_elem_value_get_enumerated(control, idx);
			break;

		default:
			DPRINTF("type not support\n");
			break;
		}
	}

	*I2CRegData = value;
	snd_hctl_close(hctl);
	return 0;
}

void LevanteI2CWrite(unsigned char I2CRegAddr, unsigned char I2CRegData)
{
	int err;
	snd_ctl_t *handle;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *control;
	unsigned int idx, count;

	snd_ctl_elem_info_t **pinfo = &info;
	snd_ctl_elem_id_t **pid = &id;
	snd_ctl_elem_value_t **pcontrol = &control;

	int numid = 0;
	int value = 0;

	numid = I2CRegAddr;
	value = I2CRegData;

	if ( (numid >= LEVANTE_AUDIO_REG_BASE1) && (numid <= LEVANTE_AUDIO_REG_END1) )
	{
		numid = numid - LEVANTE_AUDIO_REG_BASE1 + LEVANTE_CODEC_REG_BASE1;
	}
	else if ( (numid >= LEVANTE_AUDIO_REG_BASE2) && (numid <= LEVANTE_AUDIO_REG_END2) )
	{
		numid = numid - LEVANTE_AUDIO_REG_BASE2 + LEVANTE_CODEC_REG_BASE2;
	}
	else
		DPRINTF("LevanteI2CWrite; Out of range ERROR; numid= %d\n", numid);

	numid += NUMID_OFFSET;

	snd_ctl_elem_type_t type;
	snd_ctl_elem_info_alloca(pinfo);
	snd_ctl_elem_id_alloca(pid);
	snd_ctl_elem_value_alloca(pcontrol);

	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_numid(id, numid);

	if ((err = snd_ctl_open(&handle, card, 0)) < 0)
	{
		DPRINTF("LevanteI2CWrite:: snd_ctl_open error, cannot open the default card\n");
		return;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(handle, info)) < 0)
	{
		DPRINTF("LevanteI2CWrite::snd_ctl_elem_info error4, err is %d\n", err);
		return;
	}

	snd_ctl_elem_info_get_id(info, id);
	type = snd_ctl_elem_info_get_type(info);
	count = snd_ctl_elem_info_get_count(info);
	snd_ctl_elem_value_set_id(control, id);

	for (idx = 0; idx < count && idx < 128; idx++)
	{
		switch (type)
		{

		case SND_CTL_ELEM_TYPE_INTEGER:
			if ( (value >= snd_ctl_elem_info_get_min(info)) &&
			     (value <= snd_ctl_elem_info_get_max(info)) )
				snd_ctl_elem_value_set_integer(control, idx, value);
			break;


		case SND_CTL_ELEM_TYPE_ENUMERATED:
			if ( (value <= (int)(snd_ctl_elem_info_get_items(info) - 1)) && (value >= 0))
				snd_ctl_elem_value_set_enumerated(control, idx, value);
			break;

		default:
			DPRINTF("LevanteI2CWrite:: type not support\n");
			break;
		}
	}

	if ((err = snd_ctl_elem_write(handle, control)) < 0)
	{
		DPRINTF("LevanteI2CWrite:: snd_ctl_elem_write error, err is %d\n", err);
		return;
	}

	snd_ctl_close(handle);
	return;
}

