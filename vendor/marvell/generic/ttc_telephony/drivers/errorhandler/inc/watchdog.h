#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/sysdev.h>
#include <linux/spinlock.h>
#include <linux/notifier.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/list.h>
#include <linux/notifier.h>
#include <linux/relay.h>
#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/mtd/super.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/types.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 28)
#include <mach/hardware.h>
#include <mach/irqs.h>
#else
#include <asm/arch/hardware.h>
#include <asm/arch/irqs.h>
#endif
#include <asm/atomic.h>
#include <asm/io.h>

#define	WATCHDOG_BASE_ADDR	0xfe080000
#define	TMR_WMER				0xfe080064
#define	TMR_WMR				0xfe080068
#define	TMR_WVR				0xfe08006c
#define	TMR_WSR				0xfe080070

#define	TMR_WICR				0xfe080080
#define	TMR_CER				0xfe080084
#define	TMR_CMR				0xfe080088

#define	TMR_WCR				0xfe080098
#define	TMR_WFAR				0xfe08009c
#define	TMR_WSAR				0xfe0800a0

#define	WATCHDOG_1ST_ACCESS_KEY		0xbaba
#define	WATCHDOG_2ND_ACCESS_KEY	0xeb10

#define WATCHDOG_COUNT_MASK			0x1
#define WATCHDOG_RESET_MASK			0x2

#define WATCHDOG_WRITE_ACCESS_ENABLE	{ writel(WATCHDOG_1ST_ACCESS_KEY, TMR_WFAR); \
												writel(WATCHDOG_2ND_ACCESS_KEY, TMR_WSAR);}

typedef enum
{
	SUCCESSFUL,
	ERROR
} WATCHDOG_HW_RETURN_CODE;
extern void watchDogCountStop(void);
WATCHDOG_HW_RETURN_CODE watchDogDeactive(void);
