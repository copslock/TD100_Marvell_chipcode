/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/poll.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <alsa/asoundlib.h>
#include <alsa/control_external.h>

#ifdef BIONIC
#include <utils/Log.h>
#define LOG_TAG "ALSA_PLUGIN--PHONE"
#else
#define LOGE(format, arg ...) printf(format, ##arg)
#define LOGI(format, arg ...) printf(format, ##arg)
#endif

#include "audio_ipc.h"
#include "audio_protocol.h"
#include "MarvellAmixer.h"


/* debug */
#define CTL_DEBUG 0
#if CTL_DEBUG
#define dbg(format, arg ...) printf("ctl_phone: " format "\n", ## arg)
#else
#define dbg(format, arg ...)
#endif

#define PATH_CONTROL    0
#define VOLUME_CONTROL  1

#define MSASETTINGDATALEN 256


typedef struct snd_ctl_phone {
	snd_ctl_ext_t ext;
	int num_path_items;
} snd_ctl_phone_t;


static const char *mixer_dev[MIXER_NUM] = {
	//For old generic platform
	"Voice Path Control",                           //0
	"Voice Volume Control",                         //1

	//For new platform
	"Enable HIFI Speaker",                          //2
	"Disable HIFI Speaker",                         //3
	"Volume Control HIFI Speaker",                  //4
	"Mute Control HIFI Speaker",                    //5
	"Enable Voice Speaker",                         //6
	"Disable Voice Speaker",                        //7
	"Volume Control Voice Speaker",                 //8
	"Mute Control Voice Speaker",                   //9
	"Enable FMradio Speaker",                       //10
	"Disable FMradio Speaker",                      //11
	"Volume Control FMradio Speaker",               //12
	"Mute Control FMradio Speaker",                 //13

	"Enable HIFI Earpiece",                         //14
	"Disable HIFI Earpiece",                        //15
	"Volume Control HIFI Earpiece",                 //16
	"Mute Control HIFI Earpiece",                   //17
	"Enable Voice Earpiece",                        //18
	"Disable Voice Earpiece",                       //19
	"Volume Control Voice Earpiece",                //20
	"Mute Control Voice Earpiece",                  //21
	"Enable FMradio Earpiece",                      //22
	"Disable FMradio Earpiece",                     //23
	"Volume Control FMradio Earpiece",              //24
	"Mute Control FMradio Earpiece",                //25

	"Enable HIFI Headset",                          //26
	"Disable HIFI Headset",                         //27
	"Volume Control HIFI Headset",                  //28
	"Mute Control HIFI Headset",                    //29
	"Enable Voice Headset",                         //30
	"Disable Voice Headset",                        //31
	"Volume Control Voice Headset",                 //32
	"Mute Control Voice Headset",                   //33
	"Enable FMradio Headset",                       //34
	"Disable FMradio Headset",                      //35
	"Volume Control FMradio Headset",               //36
	"Mute Control FMradio Headset",                 //37

	"Enable HIFI Bluetooth",                        //38
	"Disable HIFI Bluetooth",                       //39
	"Volume Control HIFI Bluetooth",                //40
	"Mute Control HIFI Bluetooth",                  //41
	"Enable Voice Bluetooth",                       //42
	"Disable Voice Bluetooth",                      //43
	"Volume Control Voice Bluetooth",               //44
	"Mute Control Voice Bluetooth",                 //45
	"Enable FMradio Bluetooth",                     //46
	"Disable FMradio Bluetooth",                    //47
	"Volume Control FMradio Bluetooth",		//48
	"Mute Control FMradio Bluetooth",               //49

	"Enable HIFI Headphone",			//50
	"Disable HIFI Headphone",			//51
	"Volume Control HIFI Headphone",		//52
	"Mute Control HIFI Headphone",			//53
	"Enable Voice Headphone",			//54
	"Disable Voice Headphone",			//55
	"Volume Control Voice Headphone",		//56
	"Mute Control Voice Headphone", 		//57
	"Enable FMradio Headphone",			//58
	"Disable FMradio Headphone",			//59
	"Volume Control FMradio Headphone",		//60
	"Mute Control FMradio Bluetooth",		//61

	// for swithcing sample rate, using 8k in vt case, using 44.1k in HIFI case
	"switch sample rate",					//62
	//For switching calibration state, 0 indicate normal state, 1 indicate calibration state
	"swicth calibrate state",				//63

	//wrapper enable path as diag command for loopback
	"enable path",					//64
	//wrapper disable path as diag command for loopback
	"disable path",					//65

	//enable vocie bt nrec
	"Enable voice BT NREC",                        //66
	//disable vocie bt nrec
	"Disable voice BT NREC",                        //67
	"Volume Control Voice BT NREC",               //68
	"Mute Control Voice BT NREC",                 //69

	//enable force speaker
	"Enable HIFI Force Speaker",                  //70
	"Disable HIFI Force Speaker",                 //71
	"Volume Control HIFI Force Speaker",          //72
	"Mute Control HIFI Force Speaker",            //73

	//For UI calibration test
	"UI Calibration Send Settings",                 //74
};

static const char *voicepath_items[] = {
	"Phone no action",
	"Disable Voicecall",
	"Enable Voicecall",
	"Disable Pcmstream Playback",
	"Enable Pcmstream Playback",
	"Disable Pcmstrean Capture",
	"Enable Pcmstream Capture",
	"Disable HIFI Playback",
	"Enable HIFi Playback",
	"Disable HIFI Capture",
	"Enable HIFI Capture"
};

static void send_command(AUDIOIPCHEADER *header, AUDIOIPCDATA *data)
{
	int sockfd = make_client_mainsock();
	int reply;
	int ret;

	if (send_socket(sockfd, header, sizeof(AUDIOIPCHEADER)) == sizeof(AUDIOIPCHEADER))
	{
		if (send_socket(sockfd, data, sizeof(AUDIOIPCDATA)) == sizeof(AUDIOIPCDATA))
		{
			if ((ret = recv_socket(sockfd, &reply, sizeof(int))) != sizeof(int))
			{
				LOGE("************Unable to read reply from audio server, ret is %d, errno is %d\n", ret, errno);
			}
		}
		else
		{
			LOGE("Unable to send data to audio server\n");
		}
	}
	else
	{
		LOGE("Unable to set new value to audio server\n");
	}
	close_socket(sockfd);
}

static void send_ipc_data(int client, int method_type, int method_id, int device, int param)
{
	AUDIOIPCHEADER header = INVALIDIPCHEADER;
	AUDIOIPCDATA AudioIpcData = INVALIDIPCDATA;

	header.client_type = client;
	header.method_type = method_type;

	AudioIpcData.device_type = device;
	AudioIpcData.method_id = method_id;
	AudioIpcData.param = param;
	send_command(&header, &AudioIpcData);
}


static int phone_elem_count(snd_ctl_ext_t *ext)
{
	ext = ext;
	return MIXER_NUM;
}


static int phone_elem_list(snd_ctl_ext_t *ext, unsigned int offset, snd_ctl_elem_id_t *id)
{
	ext = ext;

	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);

	if (offset < MIXER_NUM)
	{
		snd_ctl_elem_id_set_numid(id, offset);
		snd_ctl_elem_id_set_name(id, mixer_dev[offset]);
	}

	return 0;
}

static snd_ctl_ext_key_t phone_find_elem(snd_ctl_ext_t *ext,
					 const snd_ctl_elem_id_t *id)
{
	const char *name;
	int i;

	ext = ext;
	name = snd_ctl_elem_id_get_name(id);

	for (i = 0; i < MIXER_NUM; i++)
		if (strcmp(name, mixer_dev[i]) == 0)
			return i;

	return snd_ctl_elem_id_get_numid(id); /*Return numid directly --Raul*/
	return SND_CTL_EXT_KEY_NOT_FOUND;
}

static int phone_get_attribute(snd_ctl_ext_t *ext, snd_ctl_ext_key_t key,
			       int *type, unsigned int *acc, unsigned int *count)
{
	ext = ext;

	if (key == 0)
	{
		*type = SND_CTL_ELEM_TYPE_ENUMERATED;
		*acc = SND_CTL_EXT_ACCESS_READWRITE;
		*count = 1;
	}
	else if (key > 0 && key < MIXER_NUM)
	{
		*type = SND_CTL_ELEM_TYPE_INTEGER;
		*acc = SND_CTL_EXT_ACCESS_READWRITE;
		*count = 1;
	}
	else
		return -EINVAL;

	return 0;
}

static int phone_get_integer_info(snd_ctl_ext_t *ext, snd_ctl_ext_key_t key,
				  long *imin, long *imax, long *istep)
{
	ext = ext;

	if (key == 0)
	{
		LOGE("key 0 is not a integer interface!\n");
		return -EINVAL;
	}
	else if (key > 0 && key < MIXER_NUM)
	{
		*istep = 1;
		*imin = 0;
		*imax = INT_MAX;
		return 0;
	}
	else
	{
		LOGE("key %d is out of range!\n", (int)key);
		return -EINVAL;
	}

}

static int phone_read_integer(snd_ctl_ext_t *ext, snd_ctl_ext_key_t key,
			      long *value)
{
	SOCKETDATA socketdata = INVALIDSOCKETDATA;
	AUDIOMIXERDATA AudioMixerData = INVALIDAUDIOMIXERDATA;

	ext = ext;
	key = key;

	if (!value) return 0;

	*value = 0;

	socketdata.client_type = PHONE_CLIENT;
	socketdata.method_type = PHONE_PLUGIN_CTL_READ;
	int sockfd = make_client_mainsock();

	if (send_socket(sockfd, &socketdata, sizeof(socketdata)) == sizeof(socketdata))
	{
		if (recv_socket(sockfd, &AudioMixerData, sizeof(AudioMixerData)) == sizeof(AudioMixerData))
		{
			*value = AudioMixerData.volume_phone;
		}
	}
	else
	{
		LOGE("Unable to receive data from audio server \n");
	}

	close_socket(sockfd);

	return 0;
}

static int phone_write_integer(snd_ctl_ext_t *ext, snd_ctl_ext_key_t key,
			       long *value)
{
	int result = 0;
	long curvalue;

	int trans_sockfd = 0;
	int rsend;
	int i;
	char dataBuf[MSASETTINGDATALEN];
	FILE *sendFile = NULL;
	FILE *dumpFile = NULL;

	int ret;
	int reply;

	dbg("integer: key is %d, value is %d\n", key, *value);
	if (key == 1)
	{
		phone_read_integer( ext, key, &curvalue);

		if (value && *value != curvalue)
		{
			SOCKETDATA socketdata = INVALIDSOCKETDATA;
			AUDIOMIXERDATA AudioMixerData = INVALIDAUDIOMIXERDATA;

			socketdata.client_type = PHONE_CLIENT;
			socketdata.method_type = PHONE_PLUGIN_CTL_WRITE;

			int sockfd = make_client_mainsock();
			if (send_socket(sockfd, &socketdata, sizeof(socketdata)) == sizeof(socketdata))
			{
				AudioMixerData.volume_phone = *value;
				if (send_socket(sockfd, &AudioMixerData,
						sizeof(AudioMixerData)) == sizeof(AudioMixerData))
				{
					result = 1;
				}
				else
				{
					LOGE("Unable to send data to audio server\n");
				}

			}
			else
			{
				LOGE("Unable to set new value to audio server\n");
			}

			close_socket(sockfd);
		}
	}
	else if (key > 1 && key < MIXER_OLD_GENERIC_NUM + MIXER_NEW_ANDROID_NUM)
	{
		dbg("key is %d, method is %s, value is 0x%x\n", key, mixer_dev[key], *value);
		send_ipc_data(gItemCtlInfo[key].client, gItemCtlInfo[key].methodType,
			      gItemCtlInfo[key].methodId, gItemCtlInfo[key].device, *value);
	}
	//For UI calibration test
	else if (key == MIXER_OLD_GENERIC_NUM + MIXER_NEW_ANDROID_NUM)//UI calibration send settings
	{
		SOCKETDATA socketdata = INVALIDSOCKETDATA;

		socketdata.client_type = CALIBRATION_CLIENT;
		socketdata.method_type = PLUGIN_CALIBRATE_MSA;

		int sockfd = make_client_mainsock();
		if (send_socket(sockfd, &socketdata, sizeof(socketdata)) == sizeof(socketdata))
		{
			//FIXME: we need wait audio serve to listen the port
			while ((trans_sockfd = make_client_transock(TRANSOCKPORT4CALIBRATE)) <= 0)
			{
				LOGE("make_client_transock failed, try again\n");
				usleep(10000);
			}

			//send MSA setting
			if (*value == 0)
				sendFile = fopen("/data/SendSetting.raw", "rb");
			//get MSA setting
			if (*value == 1)
				sendFile = fopen("/data/GetSetting.raw", "rb");
			if (sendFile)
			{
				if (fread(dataBuf, 1, MSASETTINGDATALEN, sendFile) != MSASETTINGDATALEN)
				{
					LOGE("UI calibration read raw file failed!\n");
					return 0;
				}
				rsend = send_socket(trans_sockfd, dataBuf, MSASETTINGDATALEN);

				if (rsend > 0)
				{
					LOGI("UI calibration sent out settings\n");

					if (*value == 1)
					{
						LOGI("UI calibration get reports, trans_sockfd %d\n", trans_sockfd);

						dumpFile = fopen("/data/StatusReport.raw", "wb");

						//FIXME: Suppose we will get the status report from CP side in a short time, see audio server code
						while (1)
						{
							//FIXME: here we simply always receive reports...
							if (recv_socket(trans_sockfd, dataBuf, MSASETTINGDATALEN) != MSASETTINGDATALEN)
							{
								LOGI("UI calibration get reports: calibration thread in audio server may exit\n");
								break;
							}
							else
							{
								LOGI("$$$$$$$$$$ dump UI calibration status report $$$$$$$$$$\n");
								for (i = 0; i < MSASETTINGDATALEN; i++)
								{
									if (i % 8 == 0)
									{
										LOGI("\n");
									}
									LOGI("0x%2x  ", dataBuf[i]);
								}
								LOGI("\n");
								fwrite(dataBuf, 1, MSASETTINGDATALEN, dumpFile);
							}
						}
						fclose(dumpFile);
					}
				}
				else
				{
					LOGE("UI calibration send settings failed!\n");
					close_socket(trans_sockfd);
				}

				fclose(sendFile);
			}
			else
			{
				LOGE("UI calibration open raw file failed!\n");
				return 0;
			}
		}
		else
		{
			LOGE("Unable to set new value to audio server\n");
		}

		close_socket(trans_sockfd);
		if ((ret = recv_socket(sockfd, &reply, sizeof(int))) != sizeof(int))
			LOGE("UI calibration, unable to get reply from audioserver, ret is %d, error is %d\n", ret, errno);
		else
			LOGI("UI calibration, received the reply from audioserver\n");

		close_socket(sockfd);
	}

	return result;
}

static int phone_get_enumerated_info(snd_ctl_ext_t *ext,
				     snd_ctl_ext_key_t key,
				     unsigned int *items)
{
	snd_ctl_phone_t *ctl = ext->private_data;

	switch (key)
	{
	case 0:
		*items = ctl->num_path_items;
	case 1:
		break;

	default:
		return -EINVAL;

	}
	return 0;
}

static int phone_get_enumerated_name(snd_ctl_ext_t *ext,
				     snd_ctl_ext_key_t key,
				     unsigned int item, char *name,
				     size_t name_max_len)
{
	snd_ctl_phone_t *ctl = ext->private_data;

	switch (key)
	{
	case 0:
		if ((int)item >= ctl->num_path_items)
			return -EINVAL;
		strncpy(name, voicepath_items[item], name_max_len - 1);
		break;
	case 1:
		break;
	default:
		return -EINVAL;
	}

	name[name_max_len - 1] = 0;
	return 0;
}

static int phone_read_enumerated(snd_ctl_ext_t *ext,
				 snd_ctl_ext_key_t key,
				 unsigned int *items)
{
	SOCKETDATA socketdata = INVALIDSOCKETDATA;
	AUDIOMIXERDATA AudioMixerData = INVALIDAUDIOMIXERDATA;

	ext = ext;
	key = key;
	*items = 0;

	socketdata.client_type = PHONE_CLIENT;
	socketdata.method_type = PHONE_PLUGIN_CTL_READ;

	int sockfd = make_client_mainsock();

	if (send_socket(sockfd, &socketdata, sizeof(socketdata)) == sizeof(socketdata))
	{
		if (recv_socket(sockfd, &AudioMixerData, sizeof(AudioMixerData)) == sizeof(AudioMixerData))
		{
			*items = AudioMixerData.status_phone;
		}
	}
	else
	{
		LOGE("Unable to receive data from audio server \n");
	}

	close_socket(sockfd);

	return 0;
}

static int phone_write_enumerated(snd_ctl_ext_t *ext,
				  snd_ctl_ext_key_t key,
				  unsigned int *items)
{

	int result = 0;
	unsigned int curitem;

	phone_read_enumerated( ext, key, &curitem);

	if (items && (*items != curitem))
	{
		SOCKETDATA socketdata = INVALIDSOCKETDATA;
		AUDIOMIXERDATA AudioMixerData = INVALIDAUDIOMIXERDATA;

		socketdata.client_type = PHONE_CLIENT;
		socketdata.method_type = PHONE_PLUGIN_CTL_WRITE;
		int sockfd = make_client_mainsock();
		if (send_socket(sockfd, &socketdata, sizeof(socketdata)) == sizeof(socketdata))
		{
			AudioMixerData.status_phone = *items;
			if (send_socket(sockfd, &AudioMixerData,
					sizeof(AudioMixerData)) == sizeof(AudioMixerData))
			{
				result = 1;
			}
			else
			{
				LOGE("Unable to send data to audio server\n");
			}

		}
		else
		{
			LOGE("Unable to set new value to audio server\n");
		}

		close_socket(sockfd);
	}

	return result;
}

static void phone_subscribe_events(snd_ctl_ext_t *ext, int subscribe)
{
	ext = ext;
	subscribe = subscribe;
}

static int phone_read_event(snd_ctl_ext_t *ext, snd_ctl_elem_id_t *id,
			    unsigned int *event_mask)
{
	ext = ext;
	id = id;
	event_mask = event_mask;

	return 0;
}

static int phone_ctl_poll_descriptors_count(snd_ctl_ext_t *ext)
{
	ext = ext;
	return 0;
}

static int phone_ctl_poll_descriptors(snd_ctl_ext_t *ext, struct pollfd *pfd, unsigned int space)
{
	ext = ext;
	pfd = pfd;
	space = space;
	return 0;
}

static int phone_ctl_poll_revents(snd_ctl_ext_t *ext, struct pollfd *pfd, unsigned int nfds, unsigned short *revents)
{
	ext = ext;
	pfd = pfd;
	nfds = nfds;
	revents = revents;
	return 0;
}

static void phone_close(snd_ctl_ext_t *ext)
{
	snd_ctl_phone_t *ctl = ext->private_data;

	close_socket(ext->poll_fd);
	free(ctl);
}

static snd_ctl_ext_callback_t phone_ext_callback = {
	.elem_count		= phone_elem_count,
	.elem_list		= phone_elem_list,
	.find_elem		= phone_find_elem,
	.get_attribute		= phone_get_attribute,
	.get_integer_info	= phone_get_integer_info,
	.read_integer		= phone_read_integer,
	.write_integer		= phone_write_integer,
	.get_enumerated_info	= phone_get_enumerated_info,
	.get_enumerated_name	= phone_get_enumerated_name,
	.read_enumerated	= phone_read_enumerated,
	.write_enumerated	= phone_write_enumerated,
	.subscribe_events	= phone_subscribe_events,
	.read_event		= phone_read_event,
	.poll_descriptors_count = phone_ctl_poll_descriptors_count,
	.poll_descriptors	= phone_ctl_poll_descriptors,
	.poll_revents		= phone_ctl_poll_revents,
	.close			= phone_close,
};


SND_CTL_PLUGIN_DEFINE_FUNC(phone)
{
	snd_config_iterator_t i, next;
	int err;
	snd_ctl_phone_t *ctl;

	root = root;

	snd_config_for_each(i, next, conf)
	{
		snd_config_t *n = snd_config_iterator_entry(i);
		const char *id;

		if (snd_config_get_id(n, &id) < 0)
			continue;
		if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0)
			continue;
	}

	ctl = calloc(1, sizeof(*ctl));

	ctl->ext.version = SND_CTL_EXT_VERSION;
	ctl->ext.card_idx = 0;
	strncpy(ctl->ext.id, "ALSA-PHONE-CTL", sizeof(ctl->ext.id) - 1);
	strncpy(ctl->ext.driver, "alsa phone ctl plugin", sizeof(ctl->ext.driver) - 1);
	strncpy(ctl->ext.name, "alsa phone ctl plugin", sizeof(ctl->ext.name) - 1);
	strncpy(ctl->ext.longname, "alsa phone ctl plugin for tavor", sizeof(ctl->ext.longname) - 1);
	strncpy(ctl->ext.mixername, "alsa phone plugin mixer for tavor", sizeof(ctl->ext.mixername) - 1);
	ctl->ext.poll_fd = make_udp_mainsock();
	ctl->ext.callback = &phone_ext_callback;
	ctl->ext.private_data = ctl;

	ctl->num_path_items = 11;

	err = snd_ctl_ext_create(&ctl->ext, name, mode);
	if (err < 0)
		goto error;

	*handlep = ctl->ext.handle;


	return 0;

 error:
	if (ctl->ext.poll_fd != -1) close_socket(ctl->ext.poll_fd);
	if (ctl != NULL)
		free(ctl);

	return err;
}

SND_CTL_PLUGIN_SYMBOL(phone);

