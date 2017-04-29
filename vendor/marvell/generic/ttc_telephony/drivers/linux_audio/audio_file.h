/*
 *  Audio Server for Tavor EVB Platform
 *
 */
#ifndef __AUDIO_FILE_H__
#define __AUDIO_FILE_H__


#include "audio_control.h"

// Config files shared
void get_config_filename(char* filename, int buffersize);
void read_config_string(char* filename, char* section, char* key,
			char* returnbuffer, int buffersize, char* defvalue);
int read_config_int(char* filename, char* section, char* key, int defvalue);
int read_client_info(char* filename, int clinum, audioclient* client);
int read_runtime_info(char* filename, audioserver* server);

#endif

