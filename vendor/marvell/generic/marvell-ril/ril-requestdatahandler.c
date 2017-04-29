/* ST-Ericsson U300 RIL
**
** Copyright (C) ST-Ericsson AB 2008-2010
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
**
** Based on the Android ril daemon and reference RIL by
** The Android Open Source Project.
**
** Heavily modified for ST-Ericsson U300 modems.
** Author: Christian Bejram <christian.bejram@stericsson.com>
*/

#include <stdlib.h>
#include <string.h>
#include <telephony/ril.h>
#include <assert.h>

#include "ril-requestdatahandler.h"
#define LOG_TAG "RILV"
#include <utils/Log.h>

/* Handler functions. The names are because we cheat by including
 * ril_commands.h from rild. In here we generate local allocations
 * of the data representations, as well as free:ing them after
 * they've been handled.
 *
 * This design might not be ideal, but considering the alternatives,
 * it's good enough.
 */
static void *dummyDispatch(void *data, size_t datalen);

#define dispatchCdmaSms dummyDispatch
#define dispatchCdmaSmsAck dummyDispatch
#define dispatchCdmaBrSmsCnf dummyDispatch
#define dispatchRilCdmaSmsWriteArgs dummyDispatch

static void *dispatchCallForward(void *data, size_t datalen);
static void *dispatchDial(void *data, size_t datalen);
static void *dispatchSIM_IO(void *data, size_t datalen);
static void *dispatchSmsWrite(void *data, size_t datalen);
static void *dispatchString(void *data, size_t datalen);
static void *dispatchStrings(void *data, size_t datalen);
static void *dispatchRaw(void *data, size_t datalen);
static void *dispatchVoid(void *data, size_t datalen);
static void *dispatchGsmBrSmsCnf(void *data, size_t datalen);
static void *dispatchNetworkSelectionManual(void *data, size_t datalen);

#define dispatchInts dispatchRaw

static void dummyResponse(void);

#define responseCallForwards dummyResponse
#define responseCallList dummyResponse
#define responseCellList dummyResponse
#define responseContexts dummyResponse
#define responseInts dummyResponse
#define responseRaw dummyResponse
#define responseSIM_IO dummyResponse
#define responseSMS dummyResponse
#define responseString dummyResponse
#define responseStrings dummyResponse
#define responseVoid dummyResponse

#define responseSimStatus dummyResponse
#define responseRilSignalStrength dummyResponse
#define responseDataCallList dummyResponse
#define responseGsmBrSmsCnf dummyResponse
#define responseCdmaBrSmsCnf dummyResponse

typedef struct CommandInfo {
    int requestId;
    void *(*dispatchFunction) (void *data, size_t datalen);
    void (*responseFunction) (void);
} CommandInfo;

/* RILD made me do it! */
static CommandInfo s_commandInfo[] = {
#include <ril_commands.h>
};

#ifdef MARVELL_EXTENDED
static CommandInfo s_commandInfo_ext[] = {
#include <ril_commands_ext.h>
};
#endif

static void *dummyDispatch(void *data, size_t datalen)
{
    return 0;
}

static void dummyResponse(void)
{
    return;
}

/**
 * dupRequestData will copy the data pointed to by *data, returning a pointer
 * to a freshly allocated representation of the data.
 */
static void *dupRequestData(int requestId, void *data, size_t datalen)
{
#ifdef MARVELL_EXTENDED
    CommandInfo *ci = requestId < RIL_REQUEST_EXT_BASE ? &s_commandInfo[requestId] : &s_commandInfo_ext[requestId - RIL_REQUEST_EXT_BASE];
#else
    CommandInfo *ci = &s_commandInfo[requestId];
#endif

    return ci->dispatchFunction(data, datalen);
}


RILRequest* newRILRequest(int request, void *data, size_t datalen, RIL_Token token)
{
	RILRequest* r = malloc(sizeof(RILRequest));
	assert(r != NULL);

	/* Formulate a RILRequest and put it in the queue. */
	r->request = request;
	r->data = dupRequestData(request, data, datalen);
	r->datalen = datalen;
	r->token = token;
	return r;
}

static void *dispatchCallForward(void *data, size_t datalen)
{
    RIL_CallForwardInfo *ret = dispatchRaw(data, datalen);

    if (ret->number)
        ret->number = strdup(ret->number);

    return ret;
}

static void *dispatchDial(void *data, size_t datalen)
{
    RIL_Dial *ret = dispatchRaw(data, datalen);

    if (ret->address)
        ret->address = strdup(ret->address);

    return ret;
}

static void *dispatchSIM_IO(void *data, size_t datalen)
{
    RIL_SIM_IO *ret = dispatchRaw(data, datalen);

    if (ret->path)
        ret->path = strdup(ret->path);
    if (ret->data)
        ret->data = strdup(ret->data);
    if (ret->pin2)
        ret->pin2 = strdup(ret->pin2);

    return ret;
}

static void *dispatchSmsWrite(void *data, size_t datalen)
{
    RIL_SMS_WriteArgs *ret = dispatchRaw(data, datalen);

    if (ret->pdu)
        ret->pdu = strdup(ret->pdu);

    if (ret->smsc)
        ret->smsc = strdup(ret->smsc);

    return ret;
}

static void *dispatchString(void *data, size_t datalen)
{
    //assert(datalen == sizeof(char *));

    if (data)
        return strdup((char *) data);

    return NULL;
}

static void *dispatchStrings(void *data, size_t datalen)
{
    char **a = (char **) data;
    char **ret;
    int strCount = datalen / sizeof(char *);
    int i;

    //assert((datalen % sizeof(char *)) == 0);

    ret = malloc(strCount * sizeof(char *));
    memset(ret, 0, sizeof(char *) * strCount);

    for (i = 0; i < strCount; i++)
        if (a[i])
            ret[i] = strdup(a[i]);

    return (void *) ret;
}

static void *dispatchGsmBrSmsCnf(void *data, size_t datalen)
{
    RIL_GSM_BroadcastSmsConfigInfo **a =
        (RIL_GSM_BroadcastSmsConfigInfo **) data;
    int count;
    void **ret;
    int i;

    count = datalen / sizeof(RIL_GSM_BroadcastSmsConfigInfo *);

    ret = malloc(count * sizeof(RIL_GSM_BroadcastSmsConfigInfo *));
    memset(ret, 0, sizeof(*ret));

    for (i = 0; i < count; i++)
        if (a[i])
            ret[i] =
                dispatchRaw(a[i], sizeof(RIL_GSM_BroadcastSmsConfigInfo));

    return ret;
}

#ifdef MARVELL_EXTENDED
static void* dispatchNetworkSelectionManual(void *data, size_t datalen)
{
    RIL_OperInfo *ret = dispatchRaw(data, datalen);
    if (ret->operLongStr)
        ret->operLongStr = strdup(ret->operLongStr);
    if (ret->operShortStr)
        ret->operShortStr = strdup(ret->operShortStr);
    if (ret->operNumStr)
        ret->operNumStr = strdup(ret->operNumStr);
    return ret;
}
#endif

static void *dispatchRaw(void *data, size_t datalen)
{
    void *ret = malloc(datalen);

    memcpy(ret, data, datalen);

    return (void *) ret;
}

static void *dispatchVoid(void *data, size_t datalen)
{
    return NULL;
}

static void freeDial(void *data)
{
    RIL_Dial *d = data;

    if (d->address)
        free(d->address);

    free(d);
}

static void freeStrings(void *data, size_t datalen)
{
    int count = datalen / sizeof(char *);
    int i;

    for (i = 0; i < count; i++)
        if (((char **) data)[i])
            free(((char **) data)[i]);

    free(data);
}

static void freeGsmBrSmsCnf(void *data, size_t datalen)
{
    int count = datalen / sizeof(RIL_GSM_BroadcastSmsConfigInfo);
    int i;

    for (i = 0; i < count; i++)
        if (((RIL_GSM_BroadcastSmsConfigInfo **) data)[i])
            free(((RIL_GSM_BroadcastSmsConfigInfo **) data)[i]);

    free(data);
}

static void freeSIM_IO(void *data)
{
    RIL_SIM_IO *sio = data;

    if (sio->path)
        free(sio->path);
    if (sio->data)
        free(sio->data);
    if (sio->pin2)
        free(sio->pin2);

    free(sio);
}

static void freeSmsWrite(void *data)
{
    RIL_SMS_WriteArgs *args = data;

    if (args->pdu)
        free(args->pdu);

    if (args->smsc)
        free(args->smsc);

    free(args);
}

static void freeCallForward(void *data)
{
    RIL_CallForwardInfo *cff = data;

    if (cff->number)
        free(cff->number);

    free(cff);
}

#ifdef MARVELL_EXTENDED
static void freeNetworkSelectionManual(void *data)
{
    RIL_OperInfo *oper = data;

    if (oper->operLongStr)
        free(oper->operLongStr);
    if (oper->operShortStr)
        free(oper->operShortStr);
    if (oper->operNumStr)
        free(oper->operNumStr);
    free(oper);
}
#endif

static void freeRequestData(int requestId, void *data, size_t datalen)
{
#ifdef MARVELL_EXTENDED
    CommandInfo *ci = requestId < RIL_REQUEST_EXT_BASE ? &s_commandInfo[requestId] : &s_commandInfo_ext[requestId - RIL_REQUEST_EXT_BASE];
#else
    CommandInfo *ci = &s_commandInfo[requestId];
#endif

    if (ci->dispatchFunction == dispatchInts ||
        ci->dispatchFunction == dispatchRaw ||
        ci->dispatchFunction == dispatchString) {
        if (data)
            free(data);
    } else if (ci->dispatchFunction == dispatchStrings)
        freeStrings(data, datalen);
    else if (ci->dispatchFunction == dispatchSIM_IO)
        freeSIM_IO(data);
    else if (ci->dispatchFunction == dispatchDial)
        freeDial(data);
    else if (ci->dispatchFunction == dispatchVoid) {
    } else if (ci->dispatchFunction == dispatchCallForward)
        freeCallForward(data);
    else if (ci->dispatchFunction == dispatchSmsWrite)
        freeSmsWrite(data);
    else if (ci->dispatchFunction == dispatchGsmBrSmsCnf)
        freeGsmBrSmsCnf(data, datalen);
#ifdef MARVELL_EXTENDED
    else if (ci->dispatchFunction == dispatchNetworkSelectionManual)
        freeNetworkSelectionManual(data);
#endif
}

void freeRILRequest(RILRequest* r)
{
    freeRequestData(r->request, r->data, r->datalen);
    free(r);
}

