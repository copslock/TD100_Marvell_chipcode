/*
 *
 *  Audio Server for Tavor EVB Platform
 *
 */

#include "audio_file.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pxa_dbg.h>
#include "diag.h"
#include "diag_buff.h"
/* debug */
#define printf DPRINTF

#define AUDIOFILE_DEBUG 0
#if AUDIOFILE_DEBUG
#define dbg(format, arg ...) printf("audio_file: " format "\n", ## arg)
#else
#define dbg(format, arg ...)
#endif

#define UNUSEDPARAM(param) (void)param;

void get_config_filename(char* filename, int buffersize)
{
	UNUSEDPARAM(buffersize)

	sprintf(filename, "%s/%s", getenv("HOME"), ".audiorc");
	printf("filename is %s\n", filename);
}

void read_config_string(char* filename, char* section, char* key, char* returnbuffer, int buffersize, char* defvalue)
{
	int found = 0, error = 0;
	FILE* hFile = fopen(filename, "rt");

	returnbuffer[0] = 0;
	dbg("read_config_string: reading %s\n", filename);
	if (hFile)
	{
		// search section
		while (!error && !found && !feof(hFile))
		{
			char buffer[256], szsection[256];
			if (fgets(buffer, sizeof(buffer), hFile) == NULL)
			{
				error = 1;
				break;
			}

			dbg("read_config_string: read section %s\n", buffer);
			if (sscanf(buffer, "[%s]", szsection) == 1)
			{
				szsection[strlen(szsection) - 1] = 0;
				dbg("read_config_string: scanned %s\n", szsection);
				// Found section
				if (!strcasecmp(section, szsection))
				{
					// key search loop
					while (!error && !found && !feof(hFile))
					{
						char szkey[256], szvalue[256];
						if (fgets(buffer, sizeof(buffer), hFile) == NULL)
						{
							error = 1;
							break;
						}
						dbg("read_config_string: read key %s\n", buffer);
						// Another section name will exit the key search loop
						if (sscanf(buffer, "[%s]", szsection) == 1)
						{
							break;
						}
						// A key name
						if (sscanf(buffer, "%[^=]=%[^\n]", szkey, szvalue) > 1)
						{
							dbg("read_config_string: read %s '%s' '%s'\n", buffer, szkey, szvalue);
							// Found key
							if (!strcasecmp(key, szkey))
							{
								dbg("read_config_string: buffer=%s\n", buffer);
								strncpy(returnbuffer, szvalue, buffersize);
								returnbuffer[buffersize - 1] = 0;
								found = 1;
							}
						}
					}
				}
			}
		}
		fclose(hFile);
	}

	// Put default value
	if (!found)
	{
		strncpy(returnbuffer, defvalue, buffersize);
		returnbuffer[buffersize - 1] = 0;
	}
	//syslog(LOG_INFO, "%s [%s] '%s'='%s'", __FUNCTION__, section, key, returnbuffer);
}

int read_config_int(char* filename, char* section, char* key, int defvalue)
{
	char def[32];
	char result[512];

	sprintf((char *)def, "%d", defvalue);
	read_config_string(filename, section, key, result, sizeof(result), "");
	return (atoi(result));
}


int read_client_info(char* filename, int clinum, audioclient* client)
{
	char item[128];

	sprintf(item, "%s%d", "client", clinum);

	read_config_string(filename, item, "NAME",
			   client->cliname, sizeof(client->cliname), "");
	client->control.mono_enable = read_config_int(filename, item, "MONO", 0);
	dbg("mono_enable = %d\n", client->control.mono_enable);
	client->control.bear_enable = read_config_int(filename, item, "BEAR", 0);
	dbg("bear_enable = %d\n", client->control.bear_enable);
	client->control.stereo_enable = read_config_int(filename, item, "STEREO", 0);
	dbg("stereo_enable = %d\n", client->control.stereo_enable);
	client->control.mic1_enable = read_config_int(filename, item, "MIC1", 0);
	dbg("mic1_enable = %d\n", client->control.mic1_enable);
	client->control.mic2_enable = read_config_int(filename, item, "MIC2", 0);
	dbg("mic2_enable = %d\n", client->control.mic2_enable);
	client->control.playback_vol = read_config_int(filename, item, "PLAYBACK_VOLUME", 0);
	dbg("playback_vol = %d\n", client->control.playback_vol);
	client->control.capture_vol = read_config_int(filename, item, "CAPTURE_VOLUME", 0);
	dbg("capture_vol = %d\n", client->control.capture_vol);

	return 0;
}

int read_runtime_info(char* filename, audioserver* server)
{
	char item[] = "runtime";

	server->control.mono_enable = read_config_int(filename, item, "MONO", 0);
	dbg("mono_enable = %d\n", server->control.mono_enable);
	server->control.bear_enable = read_config_int(filename, item, "BEAR", 0);
	dbg("bear_enable = %d\n", server->control.bear_enable);
	server->control.stereo_enable = read_config_int(filename, item, "STEREO", 0);
	dbg("stereo_enable = %d\n", server->control.stereo_enable);
	server->control.mic1_enable = read_config_int(filename, item, "MIC1", 0);
	dbg("mic1_enable = %d\n", server->control.mic1_enable);
	server->control.mic2_enable = read_config_int(filename, item, "MIC2", 0);
	dbg("mic2_enable = %d\n", server->control.mic2_enable);
	server->control.playback_vol = read_config_int(filename, item, "PLAYBACK_VOLUME", 0);
	dbg("playback_vol = %d\n", server->control.playback_vol);
	server->control.capture_vol = read_config_int(filename, item, "CAPTURE_VOLUME", 0);
	dbg("capture_vol = %d\n", server->control.capture_vol);


	server->bt_detect = read_config_int(filename, item, "BT_DETECT", 0);
	dbg("bt_detect = %d\n", server->bt_detect);
	server->hp_detect = read_config_int(filename, item, "HP_DETECT", 0);
	dbg("hp_detect = %d\n", server->hp_detect);
	read_config_string(filename, item, "BTADDR", server->bt_addr, 256, "" );
	dbg("server->bt_addr : %s\n", server->bt_addr);

	return 0;
}
//ICAT EXPORTED STRUCT
typedef struct
{
	char fileName[512];
	int num;
}Audio_test_struct;

//ICAT EXPORTED STRUCT
typedef struct
{
	char Name1[512];
}Audio_testdata_struct;

//ICAT EXPORTED FUNCTION - Audio, HW, AUDIO_HW_TEST
void Audio_HW_test(void *data)
{
	Audio_test_struct ats;
	Audio_testdata_struct *arg = (Audio_testdata_struct *)data;

	sprintf(ats.fileName, "this is a audio test struct:%s",arg->Name1);
	ats.num = 228;

	DPRINTF("AUDIO DIAG Test:DO Nothing! ha ha ha:%s\n",arg->Name1);
	DIAG_FILTER(AUDIO, ACM, ACMAudioTEST, DIAG_TEST)
	diagPrintf("Audio_HW_test");

	DIAG_FILTER(AUDIO, ACM, StructAudioTEST, DIAG_TEST)
	diagStructPrintf("AUDIO: Audio_HW_test%S{Audio_test_struct}",
						 ((void*)&ats), sizeof(Audio_test_struct));
}


