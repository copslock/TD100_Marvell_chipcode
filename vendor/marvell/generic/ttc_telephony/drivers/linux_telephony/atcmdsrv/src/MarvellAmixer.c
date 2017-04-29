#include <errno.h>
#if defined(BIONIC)
#include <asoundlib.h>
#else
#include <alsa/asoundlib.h>
#endif
#include "pxa_dbg.h"
#include "MarvellAmixer.h"

const char* send_MSA_filename = "/data/SendSetting.raw";
const char* get_MSA_filename = "/data/GetSetting.raw";
const char* MSA_status_filename = "/data/StatusReport.raw";

static int control_info_to_numid(int client, int method_type, int method_id, int device)
{
	int i;

	for (i = 2; i < MIXER_NUM; i++)
		if (gItemCtlInfo[i].client == client && gItemCtlInfo[i].methodType == method_type && \
		    gItemCtlInfo[i].methodId == method_id && gItemCtlInfo[i].device == device)
			return i+1;   /*Increase amixer numid due to ALSA lib upgrade*/

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

static int send_amixer_control(int num_id, int value)
{
	char card[64] = "phone";
	int err;
	snd_ctl_t *handle;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *control;
	unsigned int idx, count;
	long tmp;
	snd_ctl_elem_type_t type;

	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_value_alloca(&control);

	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);
	snd_ctl_elem_id_set_numid(id, num_id);

	if ((err = snd_ctl_open(&handle, card, 0)) < 0)
	{
		DPRINTF("Control %s open error: %s\n", card, snd_strerror(err));
		return err;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(handle, info)) < 0)
	{
		DPRINTF("Control %s cinfo error: %s\n", card, snd_strerror(err));
		return err;
	}
	snd_ctl_elem_info_get_id(info, id); /* FIXME: Remove it when hctl find works ok !!! */
	type = snd_ctl_elem_info_get_type(info);
	count = snd_ctl_elem_info_get_count(info);
	snd_ctl_elem_value_set_id(control, id);

	if (type != SND_CTL_ELEM_TYPE_INTEGER || count != 1)
	{
		DPRINTF("Control %s cinfo error: %s\n", card, snd_strerror(err));
		return -ENOENT;
	}

	tmp = check_range(value, snd_ctl_elem_info_get_min(info), snd_ctl_elem_info_get_max(info));
	idx = 0;
	snd_ctl_elem_value_set_integer(control, idx, tmp);

	if ((err = snd_ctl_elem_write(handle, control)) < 0)
	{
		DPRINTF("Control %s element write error: %s\n", card, snd_strerror(err));
		return err;
	}

	snd_ctl_close(handle);

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
	if (sendFile == NULL)
	{
		DPRINTF("UICal_send_MSA_setting: Can't open file\n");
		return -1;
	}
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
	if (sendFile == NULL)
	{
		DPRINTF("UICal_send_MSA_request: Can't open file\n");
		return -1;
	}
	fwrite(pData, sizeof(unsigned short), MSA_DATA_LENGTH, sendFile);
	fclose(sendFile);

	if (send_amixer_control(MIXER_OLD_GENERIC_NUM + MIXER_NEW_ANDROID_NUM + 1, 1) != 0)
		return -1;
	else
		return 0;
}

/*for audio loopback test*/

static void sendDspCmdAndOrData1()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= VOICE_TEST_CONTROL;   //opcode:0x46(VOICE_TEST_CONTROL)
	cmd[1]=0x4;              //length: count in char

	cmd[2]=0x0002;
	cmd[3]=0x0300;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}


static void sendDspCmdAndOrData2()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= VOICE_TEST_CONTROL;   //opcode:0x46(VOICE_TEST_CONTROL)
	cmd[1]=0x4;              //length: count in char

	cmd[2]=0x0002;
	cmd[3]=0x0100;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}


static void sendDspCmdAndOrData3()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= SELF_INVOCATION;   //opcode:0x49(SELF_INVOCATION)
	cmd[1]=0x6;              //length: count in char

	cmd[2]=0x0001;
	cmd[3]=0x0000;
	cmd[4]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData4()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= START_VOICE_PATH;   //opcode:0x40(START_VOICE_PATH)
	cmd[1]=0x0e;             //length: count in char

	cmd[2]=0x000c;
	cmd[3]=0xccd0;
	cmd[4]=0x0000;
	cmd[5]=0x0000;
	cmd[6]=0x0007;
	cmd[7]=0x0000;
	cmd[8]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData9()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= 0x61;   //opcode:0x61
	cmd[1]=0x4;              //length: count in char

	cmd[2]=0x000c;
	cmd[3]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData5()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= VOICE_TEST_CONTROL;   //opcode:0x46(VOICE_TEST_CONTROL)
	cmd[1]=0x02;             //length: count in char

	cmd[2]=0x0712;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData6()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= VOICE_TEST_CONTROL;   //opcode:0x46(VOICE_TEST_CONTROL)
	cmd[1]=0x02;             //length: count in char

	cmd[2]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData7()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= END_VOICE_PATH;   //opcode:0x41(END_VOICE_PATH)
	cmd[1]=0x00;             //length: count in char

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

static void sendDspCmdAndOrData8()
{
	unsigned short cmd[MSA_DATA_LENGTH];
	memset(cmd, 0, MSA_DATA_LENGTH*sizeof(unsigned short));

	cmd[0]= SELF_INVOCATION;   //opcode:0x49(SELF_INVOCATION)
	cmd[1]=0x02;             //length: count in char

	cmd[2]=0x0000;

	//Send cmd to MSA...
	UICal_send_MSA_setting(cmd);
}

void start_loopback()
{
	sendDspCmdAndOrData1();
	sendDspCmdAndOrData2();
	sendDspCmdAndOrData3();
	sendDspCmdAndOrData4();
	sendDspCmdAndOrData9();
	sendDspCmdAndOrData5();
}

void stop_loopback()
{
	sendDspCmdAndOrData6();
	sendDspCmdAndOrData7();
	sendDspCmdAndOrData8();
}
