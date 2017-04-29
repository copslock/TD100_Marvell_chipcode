// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
#pragma once

#include "NVMServer_api.h"
#include "nvm_shared.h"
#include "parser.h"
#include "osa.h"

#define NVM_PRIORITY    127
#define NVM_STACK_SIZE  1024
#define NVM_CLIENT_NAME_LEN     16

class CNVMIPCServer
{
public:
/* common */
CNVMIPCServer();
~CNVMIPCServer();

/* server */
BOOL InitServer(char *name);
void DeInitServer();
void RunServer() __attribute__((noreturn));

protected:
/* common */
int hIPC[2];
char clientName[NVM_CLIENT_NAME_LEN];
NVM_CLIENT_ID clientID;

/* server */
unsigned char *buf;
OSTaskRef taskRef;
char taskStack[NVM_STACK_SIZE];
NVM_PARSER_RESULT HandleIPCReqBufferAndResponse( NVM_CLIENT_ID ClientID, PUINT8 pRxBuffer, UINT32 dwBufferLen);
};

