/* based on //device/system/reference-ril/atchannel.c
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/******************************************************************************
 * *(C) Copyright 2008 Marvell International Ltd.
 * * All Rights Reserved
 * ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "marvell-ril.h"
#include "atchannel.h"
#include "at_tok.h"

#include "misc.h"
//#include <utils/Log.h>

#define MYLOG(...) ((void)LOG(LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#define MAX_AT_RESPONSE   0x1000
#define NUM_ELEMS(x) (sizeof(x) / sizeof(x[0]))

typedef enum {
	RECV_AT_CMD,
	RECV_SMS_PDU,
	SEND_AT_CMD,
	SEND_SMS_PDU
} ATLogType;

static pthread_t s_tid_reader;
static ATUnsolHandler s_unsolHandler;

struct channel_struct
{
	int id;
	int ttyFd;
	pthread_mutex_t s_commandmutex; //used to assure just one AT cmd is processing
	pthread_cond_t s_commandcond;
	ATCommandType s_type;
	const char *s_responsePrefix;
	const char *s_smsPDU;
	ATResponse *sp_response;
	char ATBuffer[MAX_AT_RESPONSE + 1];
	char *ATBufferCur;
	int timeout_count;
};

static struct channel_struct *channels;
static int COMMAND_CHANNEL_NUMBER;
static void (*s_onTimeout)(int channelID) = NULL;
static void (*s_onReaderClosed)(void) = NULL;
static int s_readerClosed;

static void onReaderClosed();
static int writeCtrlZ(struct channel_struct *chan, const char *s);
static int writeline(struct channel_struct *chan, const char *s);

static void  AT_DUMP(ATLogType logType, const char*  buff, int channel)
{
	if (logType == SEND_AT_CMD)
	{
		LOGD("===>>[Send AT cmd][%d] %s", channel, buff);
	}
	else if (logType == SEND_SMS_PDU)
	{
		LOGD("===>>[Send SMS PDU][%d] > %s^Z\n", channel, buff);
	}
	else if (logType == RECV_AT_CMD)
	{
		LOGD("<<====[Recv AT cmd][%d] %s", channel, buff);
	}
	else if (logType == RECV_SMS_PDU)
	{
		LOGD("<<====[Recv SMS PDU][%d] %s", channel, buff);
	}
}

static void init_channel_struct(struct channel_struct *chan)
{
	chan->ttyFd = -1;
	pthread_mutex_init(&chan->s_commandmutex, NULL);
	pthread_cond_init(&chan->s_commandcond, NULL);
}

void init_all_channel_struct()
{
	int i;
	COMMAND_CHANNEL_NUMBER = get_channel_number();
	channels = calloc(COMMAND_CHANNEL_NUMBER, sizeof(struct channel_struct));
	for (i = 0; i < COMMAND_CHANNEL_NUMBER; i++)
	{
		init_channel_struct(&channels[i]);
	}
}

/** add an intermediate response to sp_response*/
static void addIntermediate(struct channel_struct *chan, const char *line)
{
	ATLine *p_new;

	p_new = (ATLine *)malloc(sizeof(ATLine));
	p_new->line = strdup(line);

	/* note: this adds to the head of the list, so the list
	   will be in reverse order of lines received. the order is flipped
	   again before passing on to the command issuer */
	p_new->p_next = chan->sp_response->p_intermediates;
	chan->sp_response->p_intermediates = p_new;
}

/**
 * returns 1 if line is a final response indicating error
 * See 27.007 annex B
 * WARNING: NO CARRIER are alyways unsolicited
 */
static int isFinalResponseError(const char *line)
{
	static const char * s_finalResponsesError[] = {
		"ERROR",
		"+CMS ERROR:",
		"+CME ERROR:",
		"NO ANSWER",
		"NO DIALTONE",
	};
	size_t i;

	for (i = 0; i < NUM_ELEMS(s_finalResponsesError); i++)
	{
		if (strStartsWith(line, s_finalResponsesError[i]))
		{
			return 1;
		}
	}

	return 0;
}

/**
 * returns 1 if line is a final response indicating success
 * See 27.007 annex B
 */
static int isFinalResponseSuccess(const char *line)
{
	static const char * s_finalResponsesSuccess[] = {
		"OK",
		"CONNECT"   /* some stacks start up data on another channel */
	};
	size_t i;

	for (i = 0; i < NUM_ELEMS(s_finalResponsesSuccess); i++)
	{
		if (strStartsWith(line, s_finalResponsesSuccess[i]))
		{
			return 1;
		}
	}

	return 0;
}

/**
 * returns 1 if line is a final response, either  error or success
 * See 27.007 annex B
 * WARNING: NO CARRIER and others are sometimes unsolicited
 */
static int isFinalResponse(const char *line)
{
	return (isFinalResponseSuccess(line) || isFinalResponseError(line));
}


/**
 * returns 1 if line is the first line in (what will be) a two-line
 * SMS unsolicited response
 */
static int isSMSUnsolicited(const char *line)
{
	static const char * s_smsUnsoliciteds[] = {
		"+CMT:",
		"+CDS:",
		"+CBM:"
	};
	size_t i;

	for (i = 0; i < NUM_ELEMS(s_smsUnsoliciteds); i++)
	{
		if (strStartsWith(line, s_smsUnsoliciteds[i]))
		{
			return 1;
		}
	}

	return 0;
}

/**
 * returns 1 if line is call related unsolicited msg. These msg should be sent within the call session/context,
 * so they are not sent via unsol channel
 */
static int isCallUnsolicited(const char *line)
{
	static const char * s_callUnsoliciteds[] = {
		"+CRING:",
		"RING",
		"NO CARRIER",
		"+CCWA:",
		"+CSSI:",
		"+CSSU:",
		"+CUSD:",
		"+CGEV:",
		"+CLIP:",
		"+CNAP:",
		"CONNECT"
	};
	size_t i;

	for (i = 0; i < NUM_ELEMS(s_callUnsoliciteds); i++)
	{
		if (strStartsWith(line, s_callUnsoliciteds[i]))
		{
			return 1;
		}
	}

	return 0;
}

/** assumes s_commandmutex is held */
static void handleFinalResponse(struct channel_struct *chan, const char *line)
{
	chan->sp_response->finalResponse = strdup(line);

	pthread_cond_signal(&chan->s_commandcond);
}

static void handleUnsolicited(struct channel_struct *chan, const char *line)
{
	if (chan->id != CHANNEL_UNSOLICITED)
	{
		if ( !isCallUnsolicited(line))
			return;
	}

	if (s_unsolHandler != NULL)
	{
		s_unsolHandler(line, NULL);
	}
}

static void processLine(struct channel_struct *chan, const char *line)
{
	MYLOG("%s entry: process line: %s", __FUNCTION__, line);
	// NO CARRIER will be regarded as unsolicited message because all ATD will received OK after call-req-cnf
	pthread_mutex_lock(&chan->s_commandmutex);
	if (chan->sp_response == NULL || strStartsWith(line, "NO CARRIER"))
	{
		/* no command pending */
		MYLOG("\t No command pending, it's unsolicited message");
		handleUnsolicited(chan, line);
	}

	else if (isFinalResponseSuccess(line))
	{
		if(chan->id != CHANNEL_DATA && strStartsWith(line, "CONNECT"))
		{
			//the CONNECT will be treated as unsolicited if it is not sent from data channel
			MYLOG("\t CONNECT on not data channel, it's unsolicited message");
			handleUnsolicited(chan, line);
		}
		else
		{
			MYLOG("\t It's succesful final response");
			chan->sp_response->success = 1;
			handleFinalResponse(chan, line);
		}
	}

	else if (isFinalResponseError(line))
	{
		MYLOG("\t It's failed final response end");
		chan->sp_response->success = 0;
		handleFinalResponse(chan, line);
	}

	else if (chan->s_smsPDU != NULL && 0 == strcmp(line, "> "))
	{
		/* See eg. TS 27.005 4.3
		 * Commands like AT+CMGS have a "> " prompt
		 */
		writeCtrlZ(chan, chan->s_smsPDU);
		chan->s_smsPDU = NULL;
	}

	else
	{
		switch (chan->s_type)
		{
		case NO_RESULT:
		{
			MYLOG("\tintermediate NO RESULT, got unsolicited message");
			handleUnsolicited(chan, line);
			break;
		}
		case NUMERIC:
		{
			MYLOG("\tintermediate NUMERIC:%s", line);
			if (chan->sp_response->p_intermediates == NULL && isdigit(line[0]))
			{
				MYLOG("\tintermediate NUMERIC add intermediate");
				addIntermediate(chan, line);
			}
			else
			{
				/* either we already have an intermediate response or the line doesn't begin with a digit */
				MYLOG("\tintermediate NUMERIC unsolicitied");
				handleUnsolicited(chan, line);
			}
			break;
		}
		case SINGLELINE:
		{
			MYLOG("\tintermediate SINGLELINE: line:%s, prefix:%s", line, chan->s_responsePrefix);
			if (chan->sp_response->p_intermediates == NULL && chan->s_responsePrefix && strStartsWith(line, chan->s_responsePrefix))
			{
				MYLOG("\tintermediate add");
				addIntermediate(chan, line);
			}
			else
			{
				/* we already have an intermediate response */
				handleUnsolicited(chan, line);
			}
			break;
		}
		case MULTILINE:
		{
			MYLOG("\tintermediate MULTILINE");
			if (strStartsWith(line, chan->s_responsePrefix))
			{
				addIntermediate(chan, line);
			}
			else
			{
				handleUnsolicited(chan, line);
			}
			break;
		}
		default: /* this should never be reached */
		{
			LOGE("Unsupported AT command type %d\n", chan->s_type);
			handleUnsolicited(chan, line);
		}
		}
	}
	MYLOG("%s exit", __FUNCTION__);
	pthread_mutex_unlock(&chan->s_commandmutex);
}


/**
 * Returns a pointer to the end of the next line
 * special-cases the "> " SMS prompt
 *
 * returns NULL if there is no complete line
 */
static char * findNextEOL(char *cur)
{
	if (cur[0] == '>' && cur[1] == ' ' && cur[2] == '\0')
	{
		/* SMS prompt character...not \r terminated */
		return cur + 2;
	}

	// Find next newline
	while (*cur != '\0' && *cur != '\r' && *cur != '\n') cur++;

	return *cur == '\0' ? NULL : cur;
}


/**
 * Reads reply from the AT channel, save reply in chan->ATBuffer.
 * Assumes it has exclusive read access to the FD
 */
static int readChannel(struct channel_struct *chan)
{
	MYLOG("%s entry,channelID:%d", __FUNCTION__, chan->id);
	ssize_t count;
	char *p_read = NULL;

	/* this is a little odd. I use *s_ATBufferCur == 0 to
	 * mean "buffer consumed completely". If it points to a character, than
	 * the buffer continues until a \0
	 */
	if (*chan->ATBufferCur == '\0')
	{
		/* empty buffer */
		chan->ATBufferCur = chan->ATBuffer;
		*chan->ATBufferCur = '\0';
		p_read = chan->ATBuffer;
	}
	else
	{
		/* there's data in the buffer from the last getline*/
		int CurIdx = chan->ATBufferCur - chan->ATBuffer;
		LOGW("%s ATBufferCur not empty, start at %d: %s", __FUNCTION__, CurIdx, chan->ATBufferCur);
		size_t len = strlen(chan->ATBufferCur);
		if (CurIdx > 0)
		{
			memmove(chan->ATBuffer, chan->ATBufferCur, len + 1);
			chan->ATBufferCur = chan->ATBuffer;
		}
		p_read = chan->ATBufferCur + len;
	}

	if (0 == MAX_AT_RESPONSE - (p_read - chan->ATBuffer))
	{
		LOGE("ERROR: Input line exceeded buffer\n");
		/* ditch buffer and start over again */
		chan->ATBufferCur = chan->ATBuffer;
		*chan->ATBufferCur = '\0';
		p_read = chan->ATBuffer;
	}

	do
	{
		MYLOG("begin read channel[%d], bufsize=%d", chan->id, MAX_AT_RESPONSE - (p_read - chan->ATBuffer));
		count = read(chan->ttyFd, p_read, MAX_AT_RESPONSE - (p_read - chan->ATBuffer));
		MYLOG("end read channel[%d], count=%ld", chan->id, count);
	} while (count < 0 && errno == EINTR);

	if (count > 0)
	{
		p_read[count] = '\0';
	}
	else
	{
		/* read error encountered or EOF reached */
		if (count == 0)
		{
			LOGE("atchannel: EOF reached");
		}
		else
		{
			LOGE("atchannel: read error %s", strerror(errno));
		}
	}
	return count;
}

/* get line from channel[channelID].ATBuffer
 * if not complete line in ATBuffer, return NULL.
 */
static const char *getline(struct channel_struct *chan)
{
	MYLOG("%s entry,channelID:%d", __FUNCTION__, chan->id);
	ssize_t count;
	char *p_eol = NULL;
	char *ret = NULL;

	if (*chan->ATBufferCur == '\0') return NULL;

	// skip over leading newlines
	while (*chan->ATBufferCur == '\r' || *chan->ATBufferCur == '\n')
	{
		chan->ATBufferCur++;
	}
	p_eol = findNextEOL(chan->ATBufferCur);

	if (p_eol)
	{
		/* a full line in the buffer. Place a \0 over the \r and return */
		ret = chan->ATBufferCur;
		*p_eol = '\0';
		chan->ATBufferCur = p_eol + 1; /* this will always be <= p_read,    */
		/* and there will be a \0 at *p_read */
		while (*chan->ATBufferCur == '\r' || *chan->ATBufferCur == '\n')
		{
			chan->ATBufferCur++;
		}
	}
	return ret;
}

static void onReaderClosed()
{
	if (s_onReaderClosed != NULL && s_readerClosed == 0)
	{
		s_readerClosed = 1;
		int i;
		for (i = 0; i < COMMAND_CHANNEL_NUMBER; i++) {
			at_channel_close(i);
			struct channel_struct *chan = &channels[i];
			pthread_mutex_lock(&chan->s_commandmutex);
			pthread_cond_signal(&chan->s_commandcond);
			pthread_mutex_unlock(&chan->s_commandmutex);
		}
		s_onReaderClosed();
	}
}

static int channelReader(struct channel_struct *chan)
{
	const char * line;

	if (readChannel(chan) <= 0) return -1;
	while (1)
	{
		line = getline(chan);
		if (line == NULL) return 0;
		AT_DUMP(RECV_AT_CMD, line, chan->id);

		if (isSMSUnsolicited(line))
		{
			char *line1;
			const char *line2;
			MYLOG("found sms unsolicited on ChannelReader");

			// The scope of string returned by 'readline()' is valid only
			// till next call to 'readline()' hence making a copy of line
			// before calling readline again.
			line1 = strdup(line);
			line2 = getline(chan);

			while (line2 == NULL)
			{
				if (readChannel(chan) <=0) {
					free(line1);
					return -1;
				}
				line2 = getline(chan);
			}
			AT_DUMP(RECV_SMS_PDU, line2, chan->id);

			if ((s_unsolHandler != NULL)  && (chan->id == CHANNEL_UNSOLICITED))
			{
				s_unsolHandler(line1, line2);
			}
			free(line1);
		}

		else
		{
			processLine(chan, line);
		}
	}
	return 0;
}


static void *readerLoop(void *arg)
{
	pthread_setname_np(pthread_self(), "RIL-readerLoop");
	ThreadContext context = {"RIL-readerLoop", -1, 0};
	setThreadContext(&context);
	/* Create fdset to be listened to */
	int maxfd = 0, readSuccess;
	int i;
	fd_set fdset, readset;

	FD_ZERO(&fdset);
	for (i = 0; i < COMMAND_CHANNEL_NUMBER; i++)
	{
		if (channels[i].ttyFd > maxfd)
		{
			maxfd = channels[i].ttyFd;
		}

		if (channels[i].ttyFd != -1)
		{
			FD_SET(channels[i].ttyFd, &fdset);
		}
		else
			goto bad_channel;
	}

	/* loop to read multi-channel */
	for (;; )
	{
		/* read multi-channel by select() */
		readset = fdset;
		do {
			i = select(maxfd + 1, &readset, NULL, NULL, NULL);
		} while (i < 0 && errno == EINTR);

		if (i < 0) break;

		/* check which fd is readable, then read it by readline(fd) */
		readSuccess = 1;
		for (i = 0; i < COMMAND_CHANNEL_NUMBER; i++)
		{
			struct channel_struct *chan = &channels[i];
			if (chan->ttyFd != -1)
			{
				if (FD_ISSET(chan->ttyFd, &readset))
				{
					int ret = channelReader(chan);
					if (ret < 0)
					{
						readSuccess = 0;
						break;
					}
				}
			}
		}
		if (readSuccess == 0) break;
	}
bad_channel:
	/* exit loop, and thread quit,
	 * callback s_onReaderClosed, which is set at_set_on_reader_closed, is called
	 */
	onReaderClosed();

	return NULL;
}

/**
 * Sends string s to the radio with a \r appended.
 * Returns AT_ERROR_* on error, 0 on success
 *
 * This function exists because as of writing, android libc does not
 * have buffered stdio.
 */
static int writeline (struct channel_struct *chan, const char *s)
{
	size_t cur = 0;
	size_t len = strlen(s);
	ssize_t written;
	int s_fd = chan->ttyFd;

	if (s_fd < 0 || s_readerClosed > 0)
	{
		return AT_ERROR_CHANNEL_CLOSED;
	}

	AT_DUMP( SEND_AT_CMD, s, chan->id );

	/* the main string */
	while (cur < len)
	{
		do {
			written = write(s_fd, s + cur, len - cur);
		} while (written < 0 && errno == EINTR);

		if (written < 0)
		{
			return AT_ERROR_GENERIC;
		}

		cur += written;
	}

	/* the \r  */

	do {
		written = write(s_fd, "\r", 1);
	} while ((written < 0 && errno == EINTR) || (written == 0));

	if (written < 0) return AT_ERROR_GENERIC;

	return 0;
}

static void clearPendingCommand(struct channel_struct *chan)
{
    if (chan->sp_response != NULL) {
        at_response_free(chan->sp_response);
    }

    chan->sp_response = NULL;
    chan->s_responsePrefix = NULL;
    chan->s_smsPDU = NULL;
}

static int writeCtrlZ (struct channel_struct *chan, const char *s)
{
	size_t cur = 0;
	size_t len = strlen(s);
	ssize_t written;
	int s_fd = chan->ttyFd;

	if (s_fd < 0 || s_readerClosed > 0) return AT_ERROR_CHANNEL_CLOSED;

	AT_DUMP( SEND_AT_CMD, s, chan->id );

	/* the main string */
	while (cur < len)
	{
		do {
			written = write(s_fd, s + cur, len - cur);
		} while (written < 0 && errno == EINTR);

		if (written < 0)
		{
			return AT_ERROR_GENERIC;
		}

		cur += written;
	}

	/* the ^Z  */
	do {
		written = write(s_fd, "\032", 1);
	} while ((written < 0 && errno == EINTR) || (written == 0));

	if (written < 0) return AT_ERROR_GENERIC;

	return 0;
}

/**
 * returns 0 on success, -1 on error
 */
int at_channel_init(ATUnsolHandler h1)
{
	int ret;
	pthread_t tid;
	pthread_attr_t attr;

	s_unsolHandler = h1;
	s_readerClosed = 0;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	ret = pthread_create(&s_tid_reader, &attr, readerLoop, &attr);

	if (ret < 0)
	{
		perror("pthread_create");
		return -1;
	}

	return 0;
}

/**
 * Starts AT handler on stream "fd'
 * returns 0 on success, -1 on error
 */
static int at_channel_struct_open(struct channel_struct *chan, int channelID, int fd)
{
	pthread_mutex_lock(&chan->s_commandmutex);
	chan->id = channelID;
	chan->ttyFd = fd;
	chan->s_responsePrefix = NULL;
	chan->s_smsPDU = NULL;
	chan->sp_response = NULL;
	chan->ATBuffer[0] = '\0';
	chan->ATBufferCur = chan->ATBuffer;
	chan->timeout_count = 0;
	pthread_mutex_unlock(&chan->s_commandmutex);
	return 0;
}

int at_channel_open(int channelID, int fd)
{
	struct channel_struct *chan = &channels[channelID];
	return at_channel_struct_open(chan, channelID, fd);
}

/* FIXME is it ok to call this from the reader and the command thread? */
static void at_channel_struct_close(struct channel_struct *chan)
{
	if (chan->ttyFd >= 0)
	{
		close(chan->ttyFd);
	}

	chan->ttyFd = -1;
}

void at_channel_close(int channelID)
{
	at_channel_struct_close(&channels[channelID]);
}

static ATResponse * at_response_new()
{
	return (ATResponse *)calloc(1, sizeof(ATResponse));
}

/* It is called manually by user only in syn case. For asyn case it is auto released in handleFinalResponse() */
void at_response_free(ATResponse *p_response)
{
	ATLine *p_line;

	if (p_response == NULL) return;

	p_line = p_response->p_intermediates;

	while (p_line != NULL)
	{
		ATLine *p_toFree;

		p_toFree = p_line;
		p_line = p_line->p_next;

		free(p_toFree->line);
		free(p_toFree);
	}

	free(p_response->finalResponse);
	free(p_response);
}

/**
 * The line reader places the intermediate responses in reverse order
 * here we flip them back
 */
static void reverseIntermediates(ATResponse *p_response)
{
	ATLine *pcur, *pnext;

	pcur = p_response->p_intermediates;
	p_response->p_intermediates = NULL;

	while (pcur != NULL)
	{
		pnext = pcur->p_next;
		pcur->p_next = p_response->p_intermediates;
		p_response->p_intermediates = pcur;
		pcur = pnext;
	}
}


static int at_send_command_full_nolock(struct channel_struct *chan, const char *command, ATCommandType type,
			  const char *responsePrefix, const char *smspdu, long long timeoutMsec, ATResponse **pp_outResponse)
{
	MYLOG("at_send_command_sync entry");
	int err = 0;

	if (chan->sp_response != NULL) {
		err = AT_ERROR_COMMAND_PENDING;
		LOGE("%s: at error command pending", __FUNCTION__);
		goto error;
	}
	err = writeline(chan, command);
	if (err < 0) goto error;

	chan->s_type = type;
	chan->s_responsePrefix = responsePrefix;
	chan->s_smsPDU = smspdu;
	chan->sp_response = at_response_new();

	while (chan->sp_response->finalResponse == NULL && s_readerClosed == 0) {
		if (timeoutMsec != 0)
			err = pthread_cond_timeout_np(&chan->s_commandcond, &chan->s_commandmutex, timeoutMsec);
		else
			err = pthread_cond_wait(&chan->s_commandcond, &chan->s_commandmutex);

		if (err == ETIMEDOUT) {
			err = AT_ERROR_TIMEOUT;
			goto error;
	        }
	}

	if (pp_outResponse == NULL) {
		at_response_free(chan->sp_response);
	} else {
		/* line reader stores intermediate responses in reverse order */
		reverseIntermediates(chan->sp_response);
		*pp_outResponse = chan->sp_response;
	}
	chan->sp_response = NULL;

	if (s_readerClosed > 0)
	{
		err = AT_ERROR_CHANNEL_CLOSED;
		goto error;
	}

	MYLOG("at_send_command_sync succ exit");
	return err;

 error:
	clearPendingCommand(chan);

	MYLOG("at_send_command_sync failure exit");
	return err;
}

int at_send_command_full(const char * command,ATCommandType type,const char * responsePrefix,const char * smspdu, long long timeoutMsec, ATResponse * * pp_outResponse)
{
	ThreadContext* context = getThreadContext();
	if (context == NULL || context->write_channel < 0) {
		LOGE("not send at command in a queue thread");
		return AT_ERROR_INVALID_THREAD;
	}

	int channelID = context->write_channel;
	struct channel_struct *chan = &channels[channelID];
	if (chan->ttyFd < 0 || s_readerClosed > 0)
		return AT_ERROR_CHANNEL_CLOSED;

	if (timeoutMsec == 0)
		timeoutMsec = context->channel_timeout_msec;

	pthread_mutex_lock(&chan->s_commandmutex);
	int err = at_send_command_full_nolock(chan, command, type, responsePrefix, smspdu, timeoutMsec, pp_outResponse);
	pthread_mutex_unlock(&chan->s_commandmutex);

	if (err == AT_ERROR_TIMEOUT && s_onTimeout != NULL) {
		chan->timeout_count++;
		if (chan->timeout_count >= 3) {
			s_onTimeout(context->write_channel);
			chan->timeout_count = 0;
		}
	} else {
		chan->timeout_count = 0;
	}
	return err;
}

int at_send_command(const char *command, ATResponse **pp_outResponse)
{
    return at_send_command_full(command, NO_RESULT, NULL, NULL, 0, pp_outResponse);
}

int at_send_command_singleline(const char *command, const char *responsePrefix, ATResponse **pp_outResponse)
{
	int err = at_send_command_full(command, SINGLELINE, responsePrefix, NULL, 0, pp_outResponse);
	if (err == 0 && pp_outResponse != NULL
            && (*pp_outResponse) != NULL
            && (*pp_outResponse)->success > 0
            && (*pp_outResponse)->p_intermediates == NULL) {
		/* Successful command must have an intermediate response. */
		at_response_free(*pp_outResponse);
		*pp_outResponse = NULL;
		err = AT_ERROR_INVALID_RESPONSE;
	}

	return err;
}

int at_send_command_numeric(const char *command, ATResponse **pp_outResponse)
{
    int err;

    err = at_send_command_full(command, NUMERIC, NULL, NULL, 0, pp_outResponse);

    if (err == 0 && pp_outResponse != NULL
            && (*pp_outResponse) != NULL
            && (*pp_outResponse)->success > 0
            && (*pp_outResponse)->p_intermediates == NULL) {
        /* Successful command must have an intermediate response. */
        at_response_free(*pp_outResponse);
        *pp_outResponse = NULL;
        err = AT_ERROR_INVALID_RESPONSE;
    }

    return err;
}

int at_send_command_multiline(const char *command, const char *responsePrefix, ATResponse **pp_outResponse)
{
    return at_send_command_full(command, MULTILINE, responsePrefix, NULL, 0, pp_outResponse);
}

int at_send_command_sms(const char *command, const char *pdu, const char *responsePrefix, ATResponse **pp_outResponse)
{
    int err;


    err = at_send_command_full(command, SINGLELINE, responsePrefix,
                               pdu, 0, pp_outResponse);

    if (err == 0 && pp_outResponse != NULL
            && (*pp_outResponse) != NULL
            && (*pp_outResponse)->success > 0
            && (*pp_outResponse)->p_intermediates == NULL) {
        /* Successful command must have an intermediate response. */
        at_response_free(*pp_outResponse);
        *pp_outResponse = NULL;
        err = AT_ERROR_INVALID_RESPONSE;
    }

    return err;
}

//Johnny: need to consider when to call it?
/** This callback is invoked on the command thread */
void at_set_on_timeout(void (*onTimeout)(int channelID))
{
	s_onTimeout = onTimeout;
}

/**
 *  This callback is invoked on the reader thread (like ATUnsolHandler)
 *  when the input stream closes before you call at_close
 *  (not when you call at_close())
 *  You should still call at_close()
 */
void at_set_on_reader_closed(void (*onClose)(void))
{
	s_onReaderClosed = onClose;
}

/**
 * Returns error code from response
 * Assumes AT+CMEE=1 (numeric) mode
 */
AT_CME_Error at_get_cme_error(const ATResponse *p_response)
{
	int ret;
	int err;
	char *p_cur;

	if (p_response->success > 0) return CME_SUCCESS;

	if (p_response->finalResponse == NULL || !strStartsWith(p_response->finalResponse, "+CME ERROR:"))
	{
		return CME_ERROR_NON_CME;
	}

	p_cur = p_response->finalResponse;
	err = at_tok_start(&p_cur);
	if (err < 0)
	{
		return CME_ERROR_NON_CME;
	}

	err = at_tok_nextint(&p_cur, &ret);
	if (err < 0)
	{
		return CME_ERROR_NON_CME;
	}

	return (AT_CME_Error)ret;
}


int at_switch_data_mode(struct psd_channel_decription *pd,const char *cmd, ATResponse **pp_outResponse)
{
	int err = -1;
	const char *line;
	int fd = -1;
	struct channel_struct channel;

	fd = open(pd->ttyName, O_RDWR);
	if (fd >= 0)
	{
		pd->fd = fd;
	}
	else
	{
		LOGE("Open data channle %s error:%s!!!!!\n", pd->ttyName, strerror(errno));

		return fd;
	}
	LOGI("enter at_switch_data_mode_ppp:open tty_name:%s,channle ID is %d\n",pd->ttyName,pd->channelID);

	struct channel_struct *chan = &channel;
	init_channel_struct(chan);
	at_channel_struct_open(chan, pd->channelID,fd);

	err = writeline(chan, cmd);
	if (err < 0)
	{
		return -1;
	}

	chan->sp_response = at_response_new();
	*pp_outResponse = chan->sp_response;
	err = -1;
	while(err != 0)
	{
		readChannel(chan);
		while(1)
		{
			line = getline(chan);
			LOGI("AT channel [%d] receive: %s",chan->id,line);
			if (line == NULL)
			{
				err = -1;
				break;
			}
			else if(isFinalResponseSuccess(line))
			{
				chan->sp_response->success = 1;
				err = 0;
				break;
			}
			else if(isFinalResponseError(line))
			{
				chan->sp_response->success = 0;
				err = 0;
				break;
			}
		}
	}
	at_channel_struct_close(chan);
	return err;
}
