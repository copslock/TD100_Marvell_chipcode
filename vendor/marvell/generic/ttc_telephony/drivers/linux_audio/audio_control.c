/*
 *
 *  Audio Server for Tavor EVB Platform
 *
 */

#include "audio_control.h"
#include <acm.h>
#include <pxa_dbg.h>

void* mymalloc(int size)
{
	char* buffer = malloc(size + 8);

	if (buffer)
	{
		unsigned int sentinel = CHECKVAL;
		*((unsigned int*)buffer) = ((unsigned int)size);
		buffer += 4;
		memcpy(buffer + size, &sentinel, sizeof(sentinel));
	}
	return buffer;
}

void myfree(void* p, int line)
{
	char* buffer = p;

	if (buffer)
	{
		unsigned int size  = *((unsigned int*)(buffer - 4));
		unsigned int check;
		memcpy(&check, buffer + size, sizeof(check));
		if (check != CHECKVAL || size > 2048)
			DPRINTF("myfree: buffer overflow line %d (size=%d check=%X)\n", line, size, check);
		buffer -= 4;
		free(buffer);
	}
}

int checkbuffer__(void* p, int line)
{
	int result = 0;
	char* buffer = p;

	if (buffer)
	{
		unsigned int size  = *((unsigned int*)(buffer - 4));
		unsigned check;
		memcpy(&check, buffer + size, sizeof(check));
		if (check != CHECKVAL || size > 2048)
		{
			DPRINTF("checkbuffer__: buffer failed check line %d (size=%d check=%X)\n", line, size, check);
			result = 1;
		}
	}
	return result;
}


/* Allocate a new device */
audioclient* audiodevicenew(char * name)
{
	UNUSEDPARAM(name)
	audioclient* client = mymalloc(sizeof(audioclient));

	if (client)
	{
		memset(client, 0, sizeof(audioclient));
		//strncpy(client->cliname, name, sizeof(client->cliname));
		//client->cliname[sizeof(client->cliname) - 1] = 0;
		pthread_mutex_init(&client->mutex, NULL);
	}
	return client;
}

/* Free a device */
void audiodevicefree(audioclient* client)
{
	if (client)
	{
		pthread_mutex_destroy(&client->mutex);
		safefree(client);
	}
}





