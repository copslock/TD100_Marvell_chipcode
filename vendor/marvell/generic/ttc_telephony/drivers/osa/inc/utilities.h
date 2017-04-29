#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "linux_types.h"

enum flash_type
{
	FLASH_TYPE_NAND,
	FLASH_TYPE_EMMC,

	FLASH_TYPE_UNKNOWN = 0xFFFF
};

int get_bspflag_from_kernel_cmdline(void);
BOOL InProduction_Mode(void);
int get_chipid(unsigned long *chipid);
int get_hardwareVersion(char *hardwareVerBuf, int maxHdVerBufSize);
enum flash_type get_flash_type(void);
int mtd_name_to_number(const char *name);

#endif /* UTILITIES_H_ */

