/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include <ctype.h>
#include <alsa/asoundlib.h>

#define NUMID_OFFSET            1
#define MICCO_AUDIO_OFFSET      0x70

#define UNUSEDPARAM(param) (void)param;

char audio_card[64] = "default";


void micco_audio_write(unsigned char addr, unsigned char data)
{

	int err;
	snd_ctl_t *handle;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *control;
	unsigned int idx, count;

	int numid = 0;
	int value = 0;

	numid = addr - MICCO_AUDIO_OFFSET + NUMID_OFFSET;
	value = data;

	snd_ctl_elem_type_t type;
	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_value_alloca(&control);


	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_numid(id, numid);

	if ((err = snd_ctl_open(&handle, audio_card, 0)) < 0)
	{
		printf("snd_ctl_open error: cannot open the default card\n");
		return ;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(handle, info)) < 0)
	{
		printf("snd_ctl_elem_info error\n");
		return ;
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
			if ( ((unsigned int)value <= (snd_ctl_elem_info_get_items(info) - 1)) && (value >= 0))
				snd_ctl_elem_value_set_enumerated(control, idx, value);
			break;

		default:
			printf("type not support\n");
			break;
		}
	}

	if ((err = snd_ctl_elem_write(handle, control)) < 0)
	{
		printf("snd_ctl_elem_write error\n");
		return ;
	}

	snd_ctl_close(handle);
	return ;
}

unsigned int micco_audio_read(unsigned char addr, unsigned char *data)
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
	int value;

	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_value_alloca(&control);
	snd_ctl_elem_id_alloca(&id);

	numid = addr - MICCO_AUDIO_OFFSET + NUMID_OFFSET;

	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_numid(id, numid);

	if ((err = snd_ctl_open(&handle, audio_card, 0)) < 0)
	{
		printf("snd_ctl_open error: cannot open the default card\n");
		return err;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(handle, info)) < 0)
	{
		printf("snd_ctl_elem_info error\n");
		return err;
	}

	snd_ctl_elem_info_get_id(info, id);
	type = snd_ctl_elem_info_get_type(info);
	count = snd_ctl_elem_info_get_count(info);
	snd_ctl_elem_value_set_id(control, id);

	snd_ctl_close(handle);


	if ((err = snd_hctl_open(&hctl, audio_card, 0)) < 0)
	{
		printf("snd_hctl_open error: cannot open the default card\n");
		return err;
	}

	if ((err = snd_hctl_load(hctl)) < 0)
	{
		printf("snd_hctl_load failed\n");
		return err;
	}

	elem = snd_hctl_find_elem(hctl, id);

	if (!elem)
	{
		printf("snd_hctl_find_elem: can not find elem\n");
		return 1;
	}

	snd_ctl_elem_info_alloca(&info);

	if ((err = snd_hctl_elem_info(elem, info)) < 0)
	{
		printf("snd_hctl_elem_info failed\n");
		return err;
	}

	count = snd_ctl_elem_info_get_count(info);
	type = snd_ctl_elem_info_get_type(info);

	if ((err = snd_hctl_elem_read(elem, control)) < 0)
	{
		printf("snd_hctl_elem_read failed\n");
		return err;
	}

	for (idx = 0; idx < count; idx++)
	{
		switch (type)
		{

		case SND_CTL_ELEM_TYPE_INTEGER:
			value = snd_ctl_elem_value_get_integer(control, idx);
			printf("value = 0x%x\n", value);
			break;


		case SND_CTL_ELEM_TYPE_ENUMERATED:
			value = snd_ctl_elem_value_get_enumerated(control, idx);
			break;

		default:
			printf("type not support\n");
			break;
		}
	}

	*data = value;
	snd_hctl_close(hctl);
	return 0;
}


int set_ap_mono(int flag)
{
	UNUSEDPARAM(flag)
	return 0;
}

int set_ap_bear(int flag)
{
	UNUSEDPARAM(flag)
	return 0;
}

int set_ap_stereo(int flag)
{
	UNUSEDPARAM(flag)
	return 0;
}

int ap_micco_init(void)
{
	micco_audio_write(0x70, 0x18 );
	micco_audio_write(0x71, 0x18 );
	micco_audio_write(0x72, 0x00 );
	micco_audio_write(0x73, 0x08 );
	micco_audio_write(0x74, 0x10 );
	micco_audio_write(0x75, 0x25 );
	micco_audio_write(0x76, 0x91 );
	micco_audio_write(0x77, 0x11 );
	micco_audio_write(0x78, 0x80 );
	micco_audio_write(0x79, 0xd2 );
	micco_audio_write(0x7a, 0xd1 );
	micco_audio_write(0x7b, 0x40 );
	micco_audio_write(0x7c, 0x00 );
	micco_audio_write(0x7d, 0x00 );
	micco_audio_write(0x7e, 0x00 );
	micco_audio_write(0x7f, 0x02 );
	micco_audio_write(0x80, 0x40 );
	micco_audio_write(0x81, 0x00 );
	micco_audio_write(0x82, 0x00 );
	micco_audio_write(0x83, 0x00 );
	micco_audio_write(0x84, 0x00 );
	micco_audio_write(0x85, 0xff );

	return 0;
}

