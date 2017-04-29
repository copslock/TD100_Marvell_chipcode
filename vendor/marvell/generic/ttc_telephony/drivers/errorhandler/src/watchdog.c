#include "watchdog.h"
#include <linux/miscdevice.h>

void watchDogCountStop(void)
{
	unsigned int value;
	printk("stop watchdog\n");
	WATCHDOG_WRITE_ACCESS_ENABLE
	value = readl(TMR_WMER);
	value &= ~WATCHDOG_COUNT_MASK ;	//distable watchdog
	writel(value,TMR_WMER);
	printk("done!\n");
}

void watchDogInterruptClear(void)
{
	printk("clear watchdog interrupt\n");
	WATCHDOG_WRITE_ACCESS_ENABLE
	writel(1,TMR_WICR);
	printk("done!\n");
}
void watchDogHWKick(void)
{
	printk("kick watchdog\n");
	WATCHDOG_WRITE_ACCESS_ENABLE
	writel(1,TMR_WCR);
	printk("dong!\n");
}
WATCHDOG_HW_RETURN_CODE watchDogDeactive(void)
{
	unsigned int wvc0, wvc1, match,wmer;
	wvc0 = readl(TMR_WVR);
	wvc1 = readl(TMR_WVR);
	match = readl(TMR_WMR);
	printk("Watch Value Before stop_2:%x %x match=%x\n",wvc0,wvc1,match);
	watchDogCountStop();//watchdog count stop
	watchDogInterruptClear();
	watchDogHWKick();
	disable_irq_nosync(IRQ_PXA910_CP2_WDT);
	wvc0 = readl(TMR_WVR);
	wvc1 = readl(TMR_WVR);
	match = readl(TMR_WMR);
	wmer = readl(TMR_WMER);
	printk("Watch Value After stop_2:%x %x match=%x wmer=%x\n",wvc0,wvc1,match, wmer);
	return SUCCESSFUL;
}

