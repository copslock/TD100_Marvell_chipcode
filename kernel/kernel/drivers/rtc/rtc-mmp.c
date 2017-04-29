/*
 * Real Time Clock interface for StrongARM SA1x00 and XScale PXA2xx
 *
 * Copyright (c) 2000 Nils Faerber
 *
 * Based on rtc.c by Paul Gortmaker
 *
 * Original Driver by Nils Faerber <nils@kernelconcepts.de>
 *
 * Modifications from:
 *   CIH <cih@coventive.com>
 *   Nicolas Pitre <nico@cam.org>
 *   Andrew Christian <andrew.christian@hp.com>
 *
 * Converted to the RTC subsystem and Driver Model
 *   by Richard Purdie <rpurdie@rpsys.net>
 *
 * Support RTC on Marvell MMP
 *   by Bin Yang <bin.yang@marvell.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/rtc.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/pm.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <mach/regs-rtc.h>

#define TIMER_FREQ		CLOCK_TICK_RATE
#define RTC_DEF_DIVIDER		(32768 - 1)
#define RTC_DEF_TRIM		0

static unsigned long epoch = 1900;	/* year corresponding to 0x00	*/
static void __iomem *rtc_base;
static int irq_1hz = -1, irq_alrm = -1;
static unsigned long rtc_freq = 1024;
static struct rtc_time rtc_alarm;
static DEFINE_SPINLOCK(mmp_rtc_lock);

static inline int rtc_periodic_alarm(struct rtc_time *tm)
{
	return  (tm->tm_year == -1) ||
		((unsigned)tm->tm_mon >= 12) ||
		((unsigned)(tm->tm_mday - 1) >= 31) ||
		((unsigned)tm->tm_hour > 23) ||
		((unsigned)tm->tm_min > 59) ||
		((unsigned)tm->tm_sec > 59);
}

/*
 * Calculate the next alarm time given the requested alarm time mask
 * and the current time.
 */
static void rtc_next_alarm_time(struct rtc_time *next, struct rtc_time *now,
	struct rtc_time *alrm)
{
	unsigned long next_time;
	unsigned long now_time;

	next->tm_year = now->tm_year;
	next->tm_mon = now->tm_mon;
	next->tm_mday = now->tm_mday;
	next->tm_hour = alrm->tm_hour;
	next->tm_min = alrm->tm_min;
	next->tm_sec = alrm->tm_sec;

	rtc_tm_to_time(now, &now_time);
	rtc_tm_to_time(next, &next_time);

	if (next_time < now_time) {
		/* Advance one day */
		next_time += 60 * 60 * 24;
		rtc_time_to_tm(next_time, next);
	}
}

static int rtc_update_alarm(struct rtc_time *alrm)
{
	struct rtc_time alarm_tm, now_tm;
	unsigned long now, time;
	int ret;

	do {
		now = RCNR;
		rtc_time_to_tm(now, &now_tm);
		rtc_next_alarm_time(&alarm_tm, &now_tm, alrm);
		ret = rtc_tm_to_time(&alarm_tm, &time);
		if (ret != 0)
			break;

		RTSR = RTSR & (RTSR_HZE|RTSR_ALE|RTSR_AL);
		RTAR = time;
	} while (now != RCNR);

	return ret;
}

static irqreturn_t mmp_rtc_interrupt(int irq, void *dev_id)
{
	struct platform_device *pdev = to_platform_device(dev_id);
	struct rtc_device *rtc = platform_get_drvdata(pdev);
	unsigned int rtsr;
	unsigned long events = 0;

	spin_lock(&mmp_rtc_lock);

	rtsr = RTSR;
	/* clear interrupt sources */
	RTSR = 0;
	RTSR = (RTSR_AL | RTSR_HZ) & (rtsr >> 2);

	/* clear alarm interrupt if it has occurred */
	if (rtsr & RTSR_AL)
		rtsr &= ~RTSR_ALE;
	RTSR = rtsr & (RTSR_ALE | RTSR_HZE);

	/* update irq data & counter */
	if (rtsr & RTSR_AL)
		events |= RTC_AF | RTC_IRQF;
	if (rtsr & RTSR_HZ)
		events |= RTC_UF | RTC_IRQF;

	rtc_update_irq(rtc, 1, events);

	if (rtsr & RTSR_AL && rtc_periodic_alarm(&rtc_alarm))
		rtc_update_alarm(&rtc_alarm);
	udelay(20);

	spin_unlock(&mmp_rtc_lock);

	return IRQ_HANDLED;
}

static int mmp_rtc_open(struct device *dev)
{
	spin_lock_irq(&mmp_rtc_lock);
	enable_irq(irq_1hz);
	enable_irq(irq_alrm);
	spin_unlock_irq(&mmp_rtc_lock);
	return 0;
}

static void mmp_rtc_release(struct device *dev)
{
	spin_lock_irq(&mmp_rtc_lock);
	disable_irq(irq_1hz);
	disable_irq(irq_alrm);
	spin_unlock_irq(&mmp_rtc_lock);
}


static int mmp_rtc_ioctl(struct device *dev, unsigned int cmd,
		unsigned long arg)
{
	unsigned long arg_val;
	switch (cmd) {
	case RTC_AIE_OFF:
		spin_lock_irq(&mmp_rtc_lock);
		RTSR &= ~RTSR_ALE;
		spin_unlock_irq(&mmp_rtc_lock);
		return 0;
	case RTC_AIE_ON:
		spin_lock_irq(&mmp_rtc_lock);
		RTSR |= RTSR_ALE;
		spin_unlock_irq(&mmp_rtc_lock);
		return 0;
	case RTC_UIE_OFF:
		spin_lock_irq(&mmp_rtc_lock);
		RTSR &= ~RTSR_HZE;
		spin_unlock_irq(&mmp_rtc_lock);
		return 0;
	case RTC_UIE_ON:
		spin_lock_irq(&mmp_rtc_lock);
		RTSR |= RTSR_HZE;
		spin_unlock_irq(&mmp_rtc_lock);
		return 0;
	case RTC_IRQP_READ:
		return put_user(rtc_freq, (unsigned long *)arg);
	case RTC_IRQP_SET:
		if (get_user(arg_val, (unsigned long __user *)arg))
			return -EFAULT;
		if (arg_val < 1 || arg_val > TIMER_FREQ)
			return -EINVAL;
		rtc_freq = arg_val;
		return 0;
	case RTC_EPOCH_READ:	/* Read the epoch.	*/
		return put_user(epoch, (unsigned long __user *)arg);
	case RTC_EPOCH_SET:	/* Set the epoch.	*/
		/* Doesn't support before 1900 */
		if (get_user(arg_val, (unsigned long __user *)arg))
			return -EFAULT;
		if (arg_val < 1900)
			return -EINVAL;
		epoch = arg_val;
		return 0;
	}
	return -ENOIOCTLCMD;
}

static int mmp_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	rtc_time_to_tm(RCNR, tm);
	return 0;
}

static int mmp_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	unsigned long time;
	int ret;

	if (tm->tm_year > 138)
		return -EINVAL;
	ret = rtc_tm_to_time(tm, &time);
	if (ret == 0)
		RCNR = time;
	/*
	 * According to spec, delay more
	 * than two 32K clock cycles
	 */
	udelay(200);
	return ret;
}

static int mmp_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	u32	rtsr;

	rtc_time_to_tm(RTAR, &rtc_alarm);
	memcpy(&alrm->time, &rtc_alarm, sizeof(struct rtc_time));
	rtsr = RTSR;
	alrm->enabled = (rtsr & RTSR_ALE) ? 1 : 0;
	alrm->pending = (rtsr & RTSR_AL) ? 1 : 0;
	return 0;
}

static int mmp_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	int ret;

	spin_lock_irq(&mmp_rtc_lock);

	memcpy(&rtc_alarm, &alrm->time, sizeof(struct rtc_time));
	ret = rtc_update_alarm(&rtc_alarm);
	if (ret == 0) {
		if (alrm->enabled)
			RTSR |= RTSR_ALE;
		else
			RTSR &= ~RTSR_ALE;
	}
	spin_unlock_irq(&mmp_rtc_lock);

	return ret;
}

static int mmp_rtc_proc(struct device *dev, struct seq_file *seq)
{
	seq_printf(seq, "trim/divider\t: 0x%08x\n", (u32) RTTR);
	seq_printf(seq, "update_IRQ\t: %s\n",
			(RTSR & RTSR_HZE) ? "yes" : "no");

	return 0;
}

static const struct rtc_class_ops mmp_rtc_ops = {
	.open = mmp_rtc_open,
	.release = mmp_rtc_release,
	.ioctl = mmp_rtc_ioctl,
	.read_time = mmp_rtc_read_time,
	.set_time = mmp_rtc_set_time,
	.read_alarm = mmp_rtc_read_alarm,
	.set_alarm = mmp_rtc_set_alarm,
	.proc = mmp_rtc_proc,
};

#ifdef CONFIG_PM
static int mmp_rtc_suspend(struct device *dev)
{
	if (device_may_wakeup(dev)) {
		enable_irq_wake(irq_alrm);
	}
	return 0;
}

static int mmp_rtc_resume(struct device *dev)
{
	if (device_may_wakeup(dev)) {
		disable_irq_wake(irq_alrm);
	}
	return 0;
}

static struct dev_pm_ops mmp_rtc_pm_ops = {
	.suspend	= mmp_rtc_suspend,
	.resume		= mmp_rtc_resume,
};
#endif

#define res_size(res)	((res)->end - (res)->start + 1)
static int mmp_rtc_probe(struct platform_device *pdev)
{
	int ret;
	struct rtc_device *rtc;
	struct device *dev = &pdev->dev;
	struct resource *res;
	struct clk *clk;

	irq_1hz = platform_get_irq(pdev, 0);
	irq_alrm = platform_get_irq(pdev, 1);
	if (irq_1hz < 0 || irq_alrm < 0) {
		dev_err(&pdev->dev, "failed to get rtc irq\n");
		irq_1hz = irq_alrm = -1;
		ret = -ENXIO;
		goto err;
	}

	clk = clk_get(&pdev->dev, "MMP-RTC");
	if (IS_ERR(clk)) {
		ret = PTR_ERR(clk);
		clk = NULL;
		goto err;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "failed to get I/O memory\n");
		ret = -ENXIO;
		goto err;
	}

	res = request_mem_region(res->start, res_size(res), pdev->name);
	if (res == NULL) {
		dev_err(&pdev->dev, "failed to request I/O memory\n");
		ret = -EBUSY;
		goto err;
	}

	rtc_base = ioremap(res->start, res_size(res));
	if (rtc_base == NULL) {
		dev_err(&pdev->dev, "failed to remap I/O memory\n");
		ret = -ENXIO;
		goto err;
	}
	ret = request_irq(irq_1hz, mmp_rtc_interrupt, IRQF_DISABLED,
				"rtc 1Hz", dev);
	if (ret) {
		dev_err(dev, "IRQ %d already in use.\n", irq_1hz);
		irq_1hz = irq_alrm = -1;
		ret = -ENXIO;
		goto err;
	}
	disable_irq(irq_1hz);

	ret = request_irq(irq_alrm, mmp_rtc_interrupt, IRQF_DISABLED,
				"rtc Alrm", dev);
	if (ret) {
		dev_err(dev, "IRQ %d already in use.\n", irq_alrm);
		irq_alrm = -1;
		ret = -ENXIO;
		goto err;
	}

	if (RTTR == 0) {
		RTTR = RTC_DEF_DIVIDER + (RTC_DEF_TRIM << 16);
		dev_warn(&pdev->dev, "warning: initializing default clock"
			"divider/trim value\n");
		/* The current RTC value probably doesn't make sense either */
		RCNR = 0;
	}

	device_init_wakeup(&pdev->dev, 1);

	rtc = rtc_device_register(pdev->name, &pdev->dev, &mmp_rtc_ops,
				THIS_MODULE);

	if (IS_ERR(rtc))
		return PTR_ERR(rtc);

	clk_enable(clk);

	platform_set_drvdata(pdev, rtc);

	return 0;

err:
	if (irq_1hz >= 0)
		free_irq(irq_1hz, dev);
	if (irq_alrm >= 0)
		free_irq(irq_alrm, dev);
	return ret;
}

static int mmp_rtc_remove(struct platform_device *pdev)
{
	struct rtc_device *rtc = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;
	struct resource *res;

	RTSR = 0;
	free_irq(irq_1hz, dev);
	free_irq(irq_alrm, dev);
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	release_mem_region(res->start, res_size(res));

	if (rtc)
		rtc_device_unregister(rtc);

	return 0;
}

static struct platform_driver mmp_rtc_driver = {
	.probe		= mmp_rtc_probe,
	.remove		= mmp_rtc_remove,
	.driver		= {
		.name		= "mmp-rtc",
#ifdef CONFIG_PM
		.pm	= &mmp_rtc_pm_ops,
#endif
	},
};

static int __init mmp_rtc_init(void)
{
	return platform_driver_register(&mmp_rtc_driver);
}

static void __exit mmp_rtc_exit(void)
{
	platform_driver_unregister(&mmp_rtc_driver);
}

module_init(mmp_rtc_init);
module_exit(mmp_rtc_exit);

MODULE_AUTHOR("Bin Yang <bin.yang@marvell.com");
MODULE_DESCRIPTION("MMP Realtime Clock Driver (RTC)");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:mmp-rtc");
