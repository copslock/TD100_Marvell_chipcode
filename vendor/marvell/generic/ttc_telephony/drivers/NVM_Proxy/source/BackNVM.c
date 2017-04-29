// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "NVMServer_defs.h"
#include "BackNVM.h"
#include "diag.h"

#define BACKNVM_FILE            "/dev/mtdblock1"                                /* ReliableData area */
#define BACKNVM_OFF                     0x200                                   /* reserve 512 bytes for IMEI */
#define BACKNVM_SIZE            (0x20000 - BACKNVM_OFF)                         /* 128 KB totally */
#define BACKNVM_OP_STORE        0
#define BACKNVM_OP_LOAD         1

/* CRC32 is used as checksum */
#define CRC32_POLY                      0xEDB88320
static UINT32 CRC32Table[256];

/* BackNVM cache in RAM */
static UINT32 lenBackNVM = 0;
static char * bufBackNVM = NULL;
static UINT32 crcBackNVM = 0;
#define UNUSEDPARAM(param) (void)param;
/* Create CRC32 table */
static void CreateCRC32Table(UINT32 poly)
{
	UINT32 i, j, n;

	for (i = 0; i < 256; i++ )
	{
		n = i;
		for (j = 0; j < 8; j++)
		{
			if ((n & 1) == 1)
				n = (n >> 1) ^ poly;
			else
				n >>= 1;
		}
		CRC32Table[i] = n;
	}
}

/* Calculate CRC32 */
static UINT32 CRC32(unsigned char *buf, UINT32 len)
{
	UINT32 i, n;

	n = 0xFFFFFFFF;
	for (i = 0; i < len; i++ )
		n = (n >> 8) ^ CRC32Table[(n & 0x000000FF) ^ *buf++];

	return ~n;
}

static void loadBackNVM(void)
{
	int fd = 0;
	char *buf = NULL;
	UINT32 ret;
	UINT32 len;
	UINT32 crc;

	do {
		/* open the BackNVM partition */
		fd = open(BACKNVM_FILE, O_RDONLY);
		if (fd <= 0)
			break;

		/* Skip IMEI */
		lseek(fd, BACKNVM_OFF, SEEK_SET);

		/* read the length */
		ret = read(fd, &len, sizeof(UINT32));
		if (ret != sizeof(UINT32))
			break;

		/* too small/large to be a valid one */
		if (len < 12 + 12 + 4 || len > BACKNVM_SIZE)
			break;

		/* allocate buffer for BackNVM cache */
		buf = malloc(len);
		if (buf == NULL)
			break;

		/* read BackNVM block 0 */
		ret = read(fd, buf, len);
		if (ret != len)
			break;

		/* read BackNVM block 0 CRC */
		ret = read(fd, &crc, sizeof(UINT32));
		if (ret != sizeof(UINT32))
			break;

		/* close the BackNVM partition */
		close(fd);

		/* update BackNVM cache */
		lenBackNVM = len;
		bufBackNVM = buf;
		crcBackNVM = crc;

		return;
	} while (0);

	/* error exit */
	if (buf != NULL)
		free(buf);
	if (fd > 0)
		close(fd);
}

/* Store an NVM file from buf */
static UINT32 storeNVMFile(char *filename, char *buf, UINT32 len)
{
	UINT32 ret;
	FILE *fp;

	if (buf == NULL || len == 0)
		return 0;

	fp = fopen(filename, "w");
	if (fp == NULL)
		return 0;

	ret = fwrite(buf, 1, len, fp);

	fclose(fp);

	return ret;
}

/* Load an NVM file to *pbuf which should be freed by the caller */
static UINT32 loadNVMFile(char *filename, char **pbuf)
{
	INT32 len;
	FILE *fp;
	char *buf = NULL;

	if (pbuf == NULL)
		return 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
		return 0;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	rewind(fp);
	if (len > 0)
	{
		buf = malloc(len + 1);
		if (buf == NULL)
		{
			len = 0;
		}
		else if (fread(buf, 1, len, fp) != (UINT32)len)
		{
			len = 0;
			free(buf);
			*pbuf = NULL;
		}
		else
		{
			buf[len] = 0;
			*pbuf = buf;
		}
	}

	fclose(fp);

	return len;
}

/* Compare the BackNVM and BackNVM.cfg */
static BOOL isBackNVMCfgValid(char *buf, UINT32 len)
{
	/* TODO: now we compare the whole file
	 * Should we check the time field only? */

	if (len != lenBackNVM)
		return FALSE;

	if (strncmp(buf, bufBackNVM, len))
		return FALSE;

	return TRUE;
}

/* Store/load BackNVM by analyzing buf which should be freed here */
static int opBackNVM(char *bufNVM, UINT32 lenNVM, int op)
{
	UNUSEDPARAM(lenNVM);
	int fd = 0;
	char *p;
	char tok[2] = "\r\n";

	char *buf = NULL;
	UINT32 len;
	UINT32 crc;
	UINT32 size = 0;
	int num = 0;

	/* open the BackNVM partition */
	if (op == BACKNVM_OP_STORE)
		fd = open(BACKNVM_FILE, O_WRONLY);
	else if (op == BACKNVM_OP_LOAD)
		fd = open(BACKNVM_FILE, O_RDONLY);
	else
		return 0;

	if (fd <= 0)
	{
		DIAG_FILTER(FDI, Transport, BackNVMOpenError, DIAG_ERROR)
		diagPrintf("Cannot open BackNVM: %s", BACKNVM_FILE);

		return 0;
	}

	/* Skip IMEI */
	lseek(fd, BACKNVM_OFF, SEEK_SET);

	/* skip time field */
	p = strtok(bufNVM, tok);

	/* deal with file list */
	while ((p = strtok(NULL, tok)) != NULL)
	{
		if (op == BACKNVM_OP_STORE)
		{
			len = loadNVMFile(p, &buf);
			if (len == 0)
				continue;

			if (size + len + 4 + 4 > BACKNVM_SIZE)
			{
				DIAG_FILTER(FDI, Transport, StoreBackNVMLimitError, DIAG_ERROR)
				diagPrintf("BackNVM limit err: %d > %d", size + len + 4 + 4, BACKNVM_SIZE);

				break;
			}

			crc = CRC32((unsigned char *)buf, len);
			if ((unsigned int)write(fd, &len, sizeof(UINT32)) != sizeof(UINT32)
			    || (unsigned int)write(fd, buf, len) != len
			    || (unsigned int)write(fd, &crc, sizeof(UINT32)) != sizeof(UINT32))
			{
				DIAG_FILTER(FDI, Transport, StoreBackNVMError, DIAG_ERROR)
				diagPrintf("Error on storing %s to BackNVM[%d]", p, num);

				break;
			}

			DIAG_FILTER(FDI, Transport, StoreBackNVMFile, DIAG_INFORMATION)
			diagPrintf("Succeeded in storing %s to BackNVM[%d]", p, num);
		}
		else
		{
			buf = NULL;
			if ((unsigned int)read(fd, &len, sizeof(UINT32)) != sizeof(UINT32)
			    || len < 12 + 12 + 4 || len > BACKNVM_SIZE
			    || (buf = malloc(len)) == NULL
			    || (unsigned int)read(fd, buf, len) != len
			    || (unsigned int)read(fd, &crc, sizeof(UINT32)) != sizeof(UINT32))
			{
				DIAG_FILTER(FDI, Transport, LoadBackNVMError, DIAG_ERROR)
				diagPrintf("Error on loading %s from BackNVM[%d]", p, num);

				/* Coverity CID=179 */
				if (buf != NULL) {
					free(buf);
					buf = NULL;
				}
				break;
			}

			if (storeNVMFile(p, buf, len) != len)
			{
				DIAG_FILTER(FDI, Transport, LoadBackNVMWarning, DIAG_INFORMATION)
				diagPrintf("Length mismatch: loading %s from BackNVM[%d]", p, num);

				/* Coverity CID=179 */
				if (buf != NULL) {
					free(buf);
					buf = NULL;
				}
			}

			DIAG_FILTER(FDI, Transport, LoadBackNVMFile, DIAG_INFORMATION)
			diagPrintf("Succeeded in loading %s from BackNVM[%d]", p, num);
		}

		/* Coverity CID=179 */
		if (buf != NULL) {
			free(buf);
			buf = NULL;
		}

		num++;
	}

	/* close the BackNVM partition */
	if (fd > 0)
		close(fd);
	if (buf != NULL)
		free(buf);

	return num;
}

#define BACKNVM_TMP     "BackNVM.tmp"
/* Update the time field of BackNVM.cfg */
static BOOL updateBackNVMCfg(void)
{
	char *line = NULL;
	FILE *fpin = NULL;
	FILE *fpout = NULL;
	time_t curtime;
	struct tm *curgmtime;

	do {
		line = malloc(NVM_FILE_NAME_LENGTH);
		if (line == NULL)
			break;

		fpin = fopen(BACKNVM_CFG, "r");
		fpout = fopen(BACKNVM_TMP, "w");
		if (fpin == NULL || fpout == NULL)
			break;

		/* time field */
		if (fgets(line, NVM_FILE_NAME_LENGTH, fpin) == NULL)
			break;

		time(&curtime);
		curgmtime = gmtime(&curtime);

		fprintf(fpout, "%d/%d/%d,%d:%d:%d\n",
			curgmtime->tm_mday,
			curgmtime->tm_mon + 1,
			curgmtime->tm_year + 1900,
			curgmtime->tm_hour,
			curgmtime->tm_min,
			curgmtime->tm_sec);

		while (fgets(line, NVM_FILE_NAME_LENGTH, fpin))
			fputs(line, fpout);

		fclose(fpin);
		fclose(fpout);
		fpin = NULL;
		fpout = NULL;

		if (rename(BACKNVM_TMP, BACKNVM_CFG) < 0)
			break;

		return TRUE;
	} while (0);

	/* error exit */
	if (line != NULL)
		free(line);
	if (fpin != NULL)
		fclose(fpin);
	if (fpout != NULL)
		fclose(fpout);

	return FALSE;
}

void BackNVMPhase1Init(void)
{
	/* Empty function now */
	return;
}

void BackNVMPhase2Init(void)
{
	BOOL b1, b2;

	/* Create CRC32 table first */
	CreateCRC32Table(CRC32_POLY);

	/* Load BackNVM into the cache in RAM */
	loadBackNVM();

	/* Check BackNVM */
	b1 = IsBackNVMValid();

	/* Check BackNVM & BackNVM.cfg */
	b2 = IsBackNVMCfgValid();

	/* Refill */
	if (b1 != FALSE && b2 == FALSE)
		LoadBackNVM();  /* Refill */
}

//ICAT EXPORTED FUNCTION - FDI,Transport,IsBackNVMValid
BOOL IsBackNVMValid(void)
{
	int year, mon, day, hour, min, sec;

	year = mon = day = hour = min = sec = 0;

	do {
		/* BackNVM should include at least "time\nBackNVM.cfg\nCRC32" */
		if (lenBackNVM < 12 + 12 + 4)
			break;

		/* Check time and the first filename */
		if (sscanf(bufBackNVM, "%d/%d/%d,%d:%d:%d\nBackNVM.cfg\n",
			   &day, &mon, &year, &hour, &min, &sec) != 6)
			break;

		/* Check the checksum */
		if (CRC32((unsigned char *)bufBackNVM, lenBackNVM) != crcBackNVM)
			break;

		DIAG_FILTER(FDI, Transport, BackNVMValid, DIAG_INFORMATION)
		diagPrintf("Valid BackNVM (CRC=%d) at %d/%d/%d,%d:%d:%d",
			   crcBackNVM, day, mon, year, hour, min, sec);

		return TRUE;
	} while (0);

	/* error exit */
	DIAG_FILTER(FDI, Transport, BackNVMInvalid, DIAG_ERROR)
	diagPrintf("Invalid BackNVM (CRC=%d) at %d/%d/%d,%d:%d:%d",
		   crcBackNVM, day, mon, year, hour, min, sec);

	return FALSE;
}

//ICAT EXPORTED FUNCTION - FDI,Transport,IsBackNVMCfgValid
BOOL IsBackNVMCfgValid(void)
{
	UINT32 len;
	char *buf = NULL;
	BOOL ret;

	/* Load BackNVM.cfg */
	len = loadNVMFile(BACKNVM_CFG, &buf);
	if (len == 0)
	{
		ret = FALSE;
	}
	else if (IsBackNVMValid() == FALSE)
	{
		ret = TRUE;
		free(buf);
	}
	else
	{
		/* Check BackNVM.cfg */
		ret = isBackNVMCfgValid(buf, len);
		free(buf);
	}

	if (ret == FALSE)
	{
		DIAG_FILTER(FDI, Transport, BackNVMCfgInvalid, DIAG_ERROR)
		diagPrintf("Invalid BackNVM.cfg");
	}
	else
	{
		DIAG_FILTER(FDI, Transport, BackNVMCfgValid, DIAG_INFORMATION)
		diagPrintf("Valid BackNVM.cfg");
	}

	return ret;
}

//ICAT EXPORTED FUNCTION - FDI,Transport,StoreBackNVM
BOOL StoreBackNVM(void)
{
	int num;
	UINT32 len;
	char *buf;

	do {
		if (updateBackNVMCfg() == FALSE)
			break;

		len = loadNVMFile(BACKNVM_CFG, &buf);
		if (len == 0)
			break;

		if ((num = opBackNVM(buf, len, BACKNVM_OP_STORE)) > 0)
		{
			/* reload BackNVM to RAM cache */
			loadBackNVM();

			/* Check BackNVM again */
			if (IsBackNVMValid() == FALSE)
				break;

			DIAG_FILTER(FDI, Transport, StoreBackNVMSuccess, DIAG_INFORMATION)
			diagPrintf("Succeeded in storing %d files to BackNVM", num);

			return TRUE;
		}
	} while (0);

	/* error exit */
	DIAG_FILTER(FDI, Transport, StoreBackNVMFailure, DIAG_ERROR)
	diagPrintf("Failed to store BackNVM");

	return FALSE;
}

//ICAT EXPORTED FUNCTION - FDI,Transport,LoadBackNVM
BOOL LoadBackNVM(void)
{
	int num;
	UINT32 len;
	char *buf;

	if (IsBackNVMValid() == TRUE)
	{
		len = lenBackNVM;
		buf = malloc(len + 1);
		if (buf != NULL)
		{
			memcpy(buf, bufBackNVM, len);
			buf[len] = 0;
			if ((num = opBackNVM(buf, len, BACKNVM_OP_LOAD)) > 0)
			{
				/* Check BackNVM.cfg again */
				if (IsBackNVMCfgValid() == TRUE)
				{
					DIAG_FILTER(FDI, Transport, LoadBackNVMSuccess, DIAG_INFORMATION)
					diagPrintf("Succeeded in loading %d files from BackNVM", num);
					free(buf);
					return TRUE;
				}
			}
			free(buf);
		}
	}

	DIAG_FILTER(FDI, Transport, LoadBackNVMFailure, DIAG_ERROR)
	diagPrintf("Failed to load BackNVM");

	return FALSE;
}

//ICAT EXPORTED FUNCTION - FDI,Transport,CreateDefaultBackNVMCfg
void CreateDefaultBackNVMCfg()
{
	FILE *fp = fopen(BACKNVM_CFG, "w");
	if(fp != NULL)
	{
		fprintf(fp, "0/0/0,0:0:0\n%s\n", BACKNVM_CFG);
		fclose(fp);

		DIAG_FILTER(FDI, Transport, CreateDefaultBackNVMCfgSuccess, DIAG_INFORMATION)
		diagPrintf("Default %s created", BACKNVM_CFG);
	}else
	{
			DIAG_FILTER(FDI,Transport,CreateDefaultBackNVMCfgFail, DIAG_INFORMATION)
			diagPrintf("Error: Create default %s BACK NVM dir fail", BACKNVM_CFG);
	}
}

