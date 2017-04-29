// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
#include <string.h>
#include <stdlib.h>
#include "NVMServer_defs.h"
#include "NVMIPCClient.h"

#define BUFSIZE READ_DATA_RESPONSE_SIZE_IN_BYTES

int main()
{
	NVM_FILE_INFO fileinfo;
	HANDLE hsearch;
	NVM_STATUS_T ret;
	UINT32 fdr, fdw;
	char bufr[BUFSIZE];
	char bufw[BUFSIZE];
	UINT32 lenr, lenw;
	int i;

	if (NVMIPCClient_InitClient(NVM_IPC_ACM) == FALSE)
	{
		printf("Error on InitClient(%s)\n", NVM_IPC_ACM);
		return 1;
	}

	printf("YYYY-MM-DD HH-MM-SS Size(byte) FileName\n");
	ret = NVMIPCClient_FileFindFirst("*", &fileinfo, &hsearch);
	while (ret == NVM_STATUS_SUCCESS)
	{
		printf("%4d-%02d-%02d %02d:%02d:%02d %10d %s\n",
		       fileinfo.date & 0xfff,
		       (fileinfo.date >> 12) & 0xf,
		       (fileinfo.date >> (12 + 4)) & 0xf,
		       fileinfo.time & 0x3f,
		       (fileinfo.date >> 6) & 0x3f,
		       (fileinfo.date >> (6 + 6)) & 0x3f,
		       fileinfo.size, fileinfo.file_name);

		strcpy(bufr, (char *)fileinfo.file_name);
		if (bufr[0] == '.' || strstr(bufr, ".dup"))
			goto find_next;

		/* copy fdr to fdw */
		strcpy(bufr, (char *)fileinfo.file_name);
		NVMIPCClient_FileOpen(bufr, "rb", &fdr);
		sprintf(bufw, "%s.dup", bufr);
		NVMIPCClient_FileOpen(bufw, "wb+", &fdw);

		do {
			if (NVMIPCClient_FileRead(fdr, 1, BUFSIZE, &lenr, bufr) != NVM_STATUS_SUCCESS)
				break;
			if (lenr == 0)
				break;
			if (NVMIPCClient_FileWrite(fdw, bufr, BUFSIZE, 1, lenr, &lenw) != NVM_STATUS_SUCCESS)
				break;
			if (lenw < lenr)
				break;
		} while (1);

		/* compare fdr to fdw */
		NVMIPCClient_FileSeek(fdr, 0, SEEK_SET);
		NVMIPCClient_FileSeek(fdw, 0, SEEK_SET);

		do {
			if (NVMIPCClient_FileRead(fdr, 1, BUFSIZE, &lenr, bufr) != NVM_STATUS_SUCCESS)
			{
				printf("error in reading fdr[%d]\n", fdr);
				break;
			}
			if (NVMIPCClient_FileRead(fdw, 1, BUFSIZE, &lenw, bufw) != NVM_STATUS_SUCCESS)
			{
				printf("error in reading fdw[%d]\n", fdw);
				break;
			}
			if (lenw != lenr)
				printf("error: lenr[%d] != lenw[%d]\n", lenr, lenw);
			for (i = 0; i < lenr; i++)
			{
				if (bufr[i] != bufw[i])
				{
					printf("error: bufr[%d] != bufw[%d]\n", i, i);
					break;
				}
			}
		} while (lenr > 0 && lenw > 0);

		NVMIPCClient_FileClose(fdr);
		NVMIPCClient_FileClose(fdw);

 find_next:
		ret = NVMIPCClient_FileFindNext(&fileinfo, hsearch);
	}
	NVMIPCClient_FileFindClose(hsearch);

	NVMIPCClient_DeInitClient();
	return 0;
}

