#include <errno.h>
#include <alsa/asoundlib.h>
#include <utils/Log.h>
#include "MarvellAmixer.h"

const char* send_MSA_filename = "/data/SendSetting.raw";
const char* get_MSA_filename = "/data/GetSetting.raw";
const char* MSA_status_filename = "/data/StatusReport.raw";
static unsigned char sample_rate = 0x76; //44.1K for playback,48K for capture by default

#define SWITCH_SAMPLERATE_NUMID 8

static const int control_info_to_numid(int client, int method_type, int method_id, int device)
{
	for (int i = 2; i < MIXER_NUM; i++)
		if (gItemCtlInfo[i].client == client && gItemCtlInfo[i].methodType == method_type && \
			gItemCtlInfo[i].methodId == method_id && gItemCtlInfo[i].device == device)
			return i + 1;

	return -1;
}

static int check_range(int val, int min, int max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

static int send_amixer_control(int num_id, int value, int open_default = 0)
{
	char card[64] = "phone";
	if (open_default == 1)
		strcpy(card, "default");
	int err;
	snd_ctl_elem_info_t *ctl_info;
	snd_ctl_elem_value_t *control;
	snd_ctl_elem_id_t *ctl_id;
	snd_ctl_t *ctl_handle;
	unsigned int idx, count;
	long tmp;
	snd_ctl_elem_type_t type;
	snd_ctl_elem_info_alloca(&ctl_info);
	snd_ctl_elem_id_alloca(&ctl_id);
	snd_ctl_elem_value_alloca(&control);

	snd_ctl_elem_id_set_interface(ctl_id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_numid(ctl_id, num_id);

	if ((err = snd_ctl_open(&ctl_handle, card, 0)) < 0) {
		LOGE("Control plug-in %s open error: %s\n", card, snd_strerror(err));
		return err;
	}

	snd_ctl_elem_info_set_id(ctl_info, ctl_id);
	if ((err = snd_ctl_elem_info(ctl_handle, ctl_info)) < 0) {
		LOGE("Control plug-in %s cinfo error: %s\n", card, snd_strerror(err));
		return err;
	}
	snd_ctl_elem_info_get_id(ctl_info, ctl_id);
	type = snd_ctl_elem_info_get_type(ctl_info);
	count = snd_ctl_elem_info_get_count(ctl_info);
	snd_ctl_elem_value_set_id(control, ctl_id);

	if (type != SND_CTL_ELEM_TYPE_INTEGER || count != 1)
	{
		LOGE("Control plug-in %s cinfo error: %s\n", card, snd_strerror(err));
		return -ENOENT;
	}

	tmp = check_range(value, snd_ctl_elem_info_get_min(ctl_info), snd_ctl_elem_info_get_max(ctl_info));

	idx = 0;
	snd_ctl_elem_value_set_integer(control, idx, tmp);

	if ((err = snd_ctl_elem_write(ctl_handle, control)) < 0) {
		LOGE("Control plug-in %s element write error: %s\n", card, snd_strerror(err));
		return err;
	}

	snd_ctl_close(ctl_handle);

	return 0;
}


int audio_route_control(int client, int method_type, int method_id, int device, int value)
{
	int numid = control_info_to_numid(client, method_type, method_id, device);
	if (numid == -1)
		return numid;

	if (send_amixer_control(numid, value) != 0)
		return -1;
	else
		return 0;
}


/*UI calibration APIs*/
int UICal_send_MSA_setting(unsigned short *pData)
{
	FILE *sendFile = NULL;
	sendFile = fopen(send_MSA_filename, "wb");
	fwrite(pData, sizeof(unsigned short), MSA_DATA_LENGTH, sendFile);
	fclose(sendFile);

	if (send_amixer_control(MIXER_OLD_GENERIC_NUM + MIXER_NEW_ANDROID_NUM + 1, 0) != 0)
		return -1;
	else
		return 0;
}


int UICal_send_MSA_request(unsigned short *pData)
{
	FILE *sendFile = NULL;
	sendFile = fopen(get_MSA_filename, "wb");
	fwrite(pData, sizeof(unsigned short), MSA_DATA_LENGTH, sendFile);
	fclose(sendFile);

	if (send_amixer_control(MIXER_OLD_GENERIC_NUM + MIXER_NEW_ANDROID_NUM + 1, 1) != 0)
		return -1;
	else
		return 0;
}


/*Levante EC path contolling*/
int mic_ec_path_enable(int value)
{
	int numid = control_info_to_numid(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, SPEAKER_DEVICE);
	if (numid == -1)
		return numid;

	LOGI("**************************enable_mic ecpath: %d\n", value);
	if (send_amixer_control(numid, value | ((INPUT | ECENABLE)<<16)) != 0)
		return -1;
	else
		return 0;
}


int mic_ec_path_disable(int value)
{
	int numid = control_info_to_numid(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, SPEAKER_DEVICE);
	if (numid == -1)
		return numid;

	LOGI("**************************disable_mic ecpath: %d\n", value);
	if (send_amixer_control(numid, value | ((INPUT | ECENABLE)<<16)) != 0)
		return -1;
	else
		return 0;
}

int headset_mic_ec_path_enable(int value)
{
	int numid = control_info_to_numid(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, HEADSET_DEVICE);
	if (numid == -1)
		return numid;

	LOGI("**************************enable_headset mic ecpath: %d\n", value);
	if (send_amixer_control(numid, value | ((INPUT | ECENABLE)<<16)) != 0)
		return -1;
	else
		return 0;
}


int headset_mic_ec_path_disable(int value)
{
	int numid = control_info_to_numid(HIFI_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, HEADSET_DEVICE);
	if (numid == -1)
		return numid;

	LOGI("**************************disable_headset mic ecpath: %d\n", value);
	if (send_amixer_control(numid, value | ((INPUT | ECENABLE)<<16)) != 0)
		return -1;
	else
		return 0;
}

/* sample rate switch */
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
int amixer_switch_sample_rate(int direction, int srADC, int srReq)
{
    int numid;
    int sample_rate_req = sample_rate & 0x0F;

    numid = control_info_to_numid(HIFI_CLIENT, PLUGIN_CTL_WRITE, 0, 0);
    if (numid == -1)
        return numid;

    switch (direction) {
    case 0: //playback
        {
            switch (srADC) {
            case 48000:
                sample_rate = (sample_rate & 0xF0) | 0x07;
                break;
            case 44100:
                sample_rate = (sample_rate & 0xF0) | 0x06;
                break;
            case 8000:
                sample_rate = (sample_rate & 0xF0) | 0x01;
                break;
            default:
                LOGE("amixer_switch_sample_rate() - unsupported playback ADC sample rate %d", srADC);
                return -1;
            }

            if(send_amixer_control(numid, (SRSWITCH << 16) | 0x100 | sample_rate) != 0)
                return -1;
            if(send_amixer_control(SWITCH_SAMPLERATE_NUMID, sample_rate, 1) != 0) //send to default plugin
                return -1;
       }
        break;
    case 1: //capture
        {
            switch (srADC) {
            case 48000:
                sample_rate = (sample_rate & 0x0F) | 0x70;
                break;
	    case 32000:
	        sample_rate = (sample_rate & 0x0F) | 0x50;
                break;
            case 16000:
                sample_rate = (sample_rate & 0x0F) | 0x30;
                break;
            case 8000:
               sample_rate = (sample_rate & 0x0F) | 0x10;
                break;
            default:
                LOGI("amixer_switch_sample_rate() - unsupported capture ADC sample rate %d, set default value 48K", srADC);
                sample_rate = (sample_rate & 0x0F) | 0x70;
            }

            switch (srReq) {
            case 48000:
                sample_rate_req |= 0x70;
                break;
            case 32000:
                sample_rate_req |= 0x50;
                break;
            case 16000:
                sample_rate_req |= 0x30;
                break;
            case 8000:
                sample_rate_req |= 0x10;
                break;
            default:
                LOGI("amixer_switch_sample_rate() - unsupported capture req sample rate %d, set default value 48K", srReq);
                sample_rate_req |= 0x70;
            }

            if(send_amixer_control(numid, (SRSWITCH << 16) | 0x200 | sample_rate) != 0)
                return -1;
            if(send_amixer_control(SWITCH_SAMPLERATE_NUMID, sample_rate_req, 1) != 0) //send to default plugin
                return -1;
        }
        break;
    default:
        LOGE("amixer_switch_sample_rate() - unsupported direction %d", direction);
        return -1;
    }

    return 0;
}
