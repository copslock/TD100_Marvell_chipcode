#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "utilities.h"
#include "pxa_dbg.h"

#define PROC_CMDLINE "/proc/cmdline"
#define CMDLINE_LEN  1024
#define CMDLINE_SPLT ' '
#define BSPFLG_NAME  "androidboot.bsp="

#define REGDUMP_FILE "/sys/power/regdump"
#define REG_LINE_LEN  1024
#define CHIPID_ADDRESS "0xfe282c00"
#define REG_ADD_CON_SPLIT ':'

#define EMMC_BOOT_FLAG "emmc_boot"

#define HARDWARE_VERSION_FILE "/proc/cpuinfo"
#define HDVER_LINE_LEN  1024
#define HDVER_KEY "Rev"
#define HDVER_KEY_VALUE_SPLIT ':'


/* MRD still need to read /proc/cmdline after we drop root permission,
 * so cache it in this function */
int get_kernel_cmdline(char *buf, int len)
{
	static char cmdline[CMDLINE_LEN];
	static int is_init = 0;
	int ret = -1;
	int fd;

	if(!buf || len <= 0) return -1;

	if(is_init)
		goto INITED;

	fd = open(PROC_CMDLINE, O_RDONLY);
	if (fd < 0)
		goto ERR_RET;

	ret = read(fd, cmdline, CMDLINE_LEN);
	close(fd);

	if(ret < 0)
		goto ERR_RET;

INITED:
	ret = strlen(cmdline) + 1;
	if(ret > len)
		ret = len;

	strncpy(buf, cmdline, ret);
	buf[ret - 1] = '\0';

	is_init = 1;

	return ret;

ERR_RET:
	return -1;
}

int get_bspflag_from_kernel_cmdline(void)
{
	char cmdline[CMDLINE_LEN];
	char *pstr;
	int ret = -1;
	static int is_init = 0;
	static int flag = 0;

	if(is_init) return flag;

	ret = get_kernel_cmdline(cmdline, CMDLINE_LEN);

	if (ret < 0)
	{
		/* read error, goto err_ret */
		goto ERR_RET;
	}

	ret = -1;
	/* find the string "androidboot.bsp=" in cmdline*/
	pstr = strstr(cmdline, BSPFLG_NAME);
	if(pstr)
	{
		pstr += strlen(BSPFLG_NAME);
		ret = atoi(pstr);
	}

	flag = ret;
	is_init = 1;

ERR_RET:
	return ret;
}

BOOL InProduction_Mode(void)
{
	if(get_bspflag_from_kernel_cmdline() > 0)
		return TRUE;
	else
		return FALSE;
}

/* function: get_chipid()
*  get chipid from regdump file.
*  return value: <0 error; 1 success
*/
int get_chipid(unsigned long *chipid)
{
	int ret = -1, len;
	FILE *fp_reg = NULL;
	char *pstr;
	char regline[REG_LINE_LEN];

	fp_reg = fopen(REGDUMP_FILE, "rb");

	if (NULL == fp_reg)
	{
		ret = -2;
		goto CH_ERR_RET;
	}

	len = strlen(CHIPID_ADDRESS);

	while(NULL != fgets(regline, REG_LINE_LEN, fp_reg))
	{
		if (!strncmp(regline, CHIPID_ADDRESS, len))
		{
			pstr = strchr(regline, REG_ADD_CON_SPLIT);
			// pstr+1: skip ':'
			*chipid = strtoul(pstr+1, 0, 16);
			ret = 1;
			break;
		}
	}
	fclose(fp_reg);
CH_ERR_RET:
	return ret;
}

/* get flash type according to kernel cmdline */
enum flash_type get_flash_type(void)
{
	char cmdline[CMDLINE_LEN];
	int ret = -1;
	static int is_init = 0;
	static enum flash_type type;

	if(is_init) return type;

	ret = get_kernel_cmdline(cmdline, CMDLINE_LEN);

	if (ret < 0)
	{
		/* read error, goto err_ret */
		goto ERR_RET;
	}

	if(strstr(cmdline, EMMC_BOOT_FLAG))
		type = FLASH_TYPE_EMMC;
	else
		type = FLASH_TYPE_NAND;

	is_init = 1;

	return type;

ERR_RET:
	return FLASH_TYPE_UNKNOWN;
}

#define MAX_MTD_PARTITIONS 16

static struct {
	char name[16];
	int number;
} mtd_part_map[MAX_MTD_PARTITIONS];

static int mtd_part_count = -1;

static void find_mtd_partitions(void)
{
	FILE *fp;
	char line[1024];

	fp = fopen("/proc/mtd", "r");
	if(fp == NULL)
		return;

	while(fgets(line, sizeof(line) / sizeof(line[0]) - 1, fp))
	{
		int num, size, erasesize;
		int ret;
		char name[16];
		name[0] = '\0';
		num = -1;
		ret = sscanf(line, "mtd%d: %x %x %15s",
			&num, &size, &erasesize, name);
		if ((ret == 4) && (name[0] == '"'))
		{
			char *x = strchr(name + 1, '"');
			if (x) *x = 0;
			DBGMSG("mtd partition %d, %s\n", num, name + 1);
			if (mtd_part_count < MAX_MTD_PARTITIONS)
			{
				strcpy(mtd_part_map[mtd_part_count].name, name + 1);
				mtd_part_map[mtd_part_count].number = num;
				mtd_part_count++;
			}
			else
			{
				ERRMSG("too many mtd partitions\n");
			}
		}

	}
	fclose(fp);
}

int mtd_name_to_number(const char *name)
{
	int n;
	if (mtd_part_count < 0)
	{
		mtd_part_count = 0;
		find_mtd_partitions();
	}
	for (n = 0; n < mtd_part_count; n++)
	{
		if (!strcmp(name, mtd_part_map[n].name))
			return mtd_part_map[n].number;
	}
	return -1;
}
/* function: get_hardwareVersion()
*  get hardware version from /proc/cpuinfo.
*  return value: <0 error; 1 success
*/
int get_hardwareVersion(char *hardwareVerBuf, int maxHdVerBufSize)
{
	int ret = -1, hdVerKeyLen, hdVerValueLen;
	FILE *fp = NULL;
	char *pstr;
	char line[HDVER_LINE_LEN];

	fp = fopen("/proc/cpuinfo", "rb");

	if (NULL == fp)
	{
		ret = -2;
		goto ERR_RET;
	}

	hdVerKeyLen = strlen(HDVER_KEY);

	while(NULL != fgets(line, HDVER_LINE_LEN, fp))
	{
		if (!strncmp(line, HDVER_KEY, hdVerKeyLen))
		{
			pstr = strchr(line, HDVER_KEY_VALUE_SPLIT);
			if(pstr == NULL)
			{
				ret = -3;
				break;
			}
			// ++pstr: skip ':'
			hdVerValueLen = strlen(++pstr);
			while(isspace(pstr[hdVerValueLen - 1]))
			{
				hdVerValueLen--;
			}
			pstr[hdVerValueLen] = '\0';
			// 1 is for the space between hdVerKey and hdVerValue
			while(isspace(*pstr) || ((hdVerKeyLen + hdVerValueLen + 1 >= maxHdVerBufSize) && (*pstr == '0')))
			{
				hdVerValueLen--;
				pstr++;
			}
			if(hdVerKeyLen + hdVerValueLen + 1 >= maxHdVerBufSize || hdVerValueLen <= 0)
			{
				ret = -4;
				break;
			}
			snprintf(hardwareVerBuf, maxHdVerBufSize, "%s %s", HDVER_KEY, pstr);
			ret = 1;
			break;
		}
	}
	fclose(fp);
ERR_RET:
	return ret;
}

