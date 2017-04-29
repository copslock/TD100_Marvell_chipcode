/*
 * Real Time Clock driver for Marvell 88PM860x PMIC
 *
 * Copyright (c) 2010 Marvell International Ltd.
 * Author:	Haojian Zhuang <haojian.zhuang@marvell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/rtc.h>
#include <linux/delay.h>
#include <linux/mfd/88pm860x.h>
#include <asm/io.h>
#include <mach/regs-rtc.h>

#define SYNC_TIME_TO_SOC

#ifdef CONFIG_RTC_MON
#include <mach/88pm860x-rtc.h>
#include <mach/regs-rtc.h>
#include <linux/miscdevice.h>
DECLARE_WAIT_QUEUE_HEAD(rtc_update_head);
#endif

#if defined(SYNC_TIME_TO_SOC) && defined(CONFIG_ARCH_MMP)


static void __iomem *rtc_base;
#endif

#if defined(SYNC_TIME_TO_SOC) && defined(CONFIG_ARCH_PXA)
extern int pxa_rtc_sync_time();
#endif

#define VRTC_CALIBRATION

struct pm860x_rtc_info {
	struct pm860x_chip	*chip;
	struct i2c_client	*i2c;
	struct rtc_device	*rtc_dev;
	struct device		*dev;

	struct delayed_work	calib_work;
	int			irq;
	int			vrtc;
};

#define REG0_ADDR		0xB0
#define REG1_ADDR		0xB2
#define REG2_ADDR		0xB4
#define REG3_ADDR		0xB6

#define REG0_DATA		0xB1
#define REG1_DATA		0xB3
#define REG2_DATA		0xB5
#define REG3_DATA		0xB7

/* bit definitions of Measurement Enable Register 2 (0x51) */
#define MEAS2_VRTC		(1 << 0)

/* bit definitions of RTC Register 1 (0xA0) */
#define ALARM_EN		(1 << 3)
#define ALARM_WAKEUP		(1 << 4)
#define ALARM			(1 << 5)
#define RTC1_USE_XO		(1 << 7)

#define VRTC_CALIB_INTERVAL	(HZ * 60 * 10)		/* 10 minutes */
#define VRTC_CALIB_TRIES	(10)

static struct platform_device *g_pdev;
static int pm860x_rtc_read_time(struct device *dev, struct rtc_time *tm);

#ifdef CONFIG_RTC_MON
static long pm860x_rtcmon_ioctl(struct file *file,
							unsigned int cmd,
							unsigned long arg)
{
	void __user *uarg = (void __user *)arg;
	struct rtc_time tm = {0};
	int ret = 0;

	lock_kernel();

	switch (cmd) {

	case RTC_CHANGE_SUB:

		{
			DEFINE_WAIT(wait);
			prepare_to_wait(&rtc_update_head,
							&wait,
							TASK_INTERRUPTIBLE);
			schedule();
			finish_wait(&rtc_update_head, &wait);
		}

		ret = pm860x_rtc_read_time(&g_pdev->dev, &tm);

		if (ret < 0)
			pr_info( \
			"pxa_rtcmon_ioctl::pm860x_rtc_read_time fail]\n");
		else
			pr_info( \
			"pxa_rtcmon_ioctl::pm860x_rtc_read_time=%d:%d:%d\n",
			tm.tm_hour, tm.tm_min, tm.tm_sec);

			ret = copy_to_user(uarg, &tm, sizeof(struct rtc_time));
			if (ret)
				ret = -EFAULT;
		break;
	default:
		pr_info( \
		"pxa_rtcmon_ioctl:default\n");
		ret = -ENOIOCTLCMD;
	}

	unlock_kernel();

	return ret;
}

static int pm860x_rtcmon_open(struct inode *inode, struct file *file)
{
	pr_info( \
	"pm860x_rtcmon_open::nothing done here\n");
	return 0;
}

static int pm860x_rtcmon_release(struct inode *inode, struct file *file)
{
	pr_info( \
	"pm860x_rtcmon_release::nothing done here\n");
	return 0;
}
#endif

static irqreturn_t rtc_update_handler(int irq, void *data)
{
	struct pm860x_rtc_info *info = (struct pm860x_rtc_info *)data;
	int mask;

	mask = ALARM | ALARM_WAKEUP;
	pm860x_set_bits(info->i2c, PM8607_RTC1, mask | ALARM_EN, mask);
	rtc_update_irq(info->rtc_dev, 1, RTC_AF);
	return IRQ_HANDLED;
}
static int pm860x_rtc_alarm_irq_enable(struct device *dev,unsigned int enabled)
{
	struct pm860x_rtc_info *info = dev_get_drvdata(dev);

	if(enabled) {
		pm860x_set_bits(info->i2c, PM8607_RTC1, ALARM, ALARM);
	} else {
		pm860x_set_bits(info->i2c, PM8607_RTC1, ALARM, 0);
	}
	return 0;
}

static int pm860x_rtc_alarm_irq_update(struct device *dev, unsigned int enabled)
{
	struct pm860x_rtc_info *info = dev_get_drvdata(dev);

	if (enabled) {
		pm860x_set_bits(info->i2c, PM8607_RTC1, ALARM_EN, ALARM_EN);
	} else {
		pm860x_set_bits(info->i2c, PM8607_RTC1, ALARM_EN, 0);
		pr_info("pm860x rtc alarm irq disable.\n");
	}
	return 0;
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

static int pm860x_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct pm860x_rtc_info *info = dev_get_drvdata(dev);
	unsigned char buf[8];
	unsigned long ticks, base, data;

	pm860x_page_bulk_read(info->i2c, REG0_ADDR, 8, buf);
	dev_dbg(info->dev, "%x-%x-%x-%x-%x-%x-%x-%x\n", buf[0], buf[1],
		buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	base = (buf[1] << 24) | (buf[3] << 16) | (buf[5] << 8) | buf[7];

	/* load 32-bit read-only counter */
	pm860x_bulk_read(info->i2c, PM8607_RTC_COUNTER1, 4, buf);
	data = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
	ticks = base + data;
	dev_dbg(info->dev, "get base:0x%lx, RO count:0x%lx, ticks:0x%lx\n",
		base, data, ticks);

	rtc_time_to_tm(ticks, tm);

#if defined(SYNC_TIME_TO_SOC)
	dev_dbg(info->dev, "RCNR:0x%x\n", RCNR);
#endif
	return 0;
}

static int pm860x_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct pm860x_rtc_info *info = dev_get_drvdata(dev);
	unsigned char buf[4];
	unsigned long ticks, base, data;

	if((tm->tm_year < 70) || (tm->tm_year > 138)) {
		dev_dbg(info->dev, "set time %d out of range, please set time between 1970 to 2038.\n",
			1900+tm->tm_year);
		return -EINVAL;
	}
	rtc_tm_to_time(tm, &ticks);

	/* load 32-bit read-only counter */
	pm860x_bulk_read(info->i2c, PM8607_RTC_COUNTER1, 4, buf);
	data = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
	base = ticks - data;
	dev_dbg(info->dev, "set base:0x%lx, RO count:0x%lx, ticks:0x%lx\n",
		base, data, ticks);

	pm860x_page_reg_write(info->i2c, REG0_DATA, (base >> 24) & 0xFF);
	pm860x_page_reg_write(info->i2c, REG1_DATA, (base >> 16) & 0xFF);
	pm860x_page_reg_write(info->i2c, REG2_DATA, (base >> 8) & 0xFF);
	pm860x_page_reg_write(info->i2c, REG3_DATA, base & 0xFF);

#if defined(SYNC_TIME_TO_SOC)
#ifdef CONFIG_ARCH_MMP
	RCNR = ticks;
#elif defined(CONFIG_ARCH_PXA)
	pxa_rtc_sync_time(&tm);
#endif

	dev_dbg(info->dev, "RCNR:0x%x\n", RCNR);
#endif

#ifdef CONFIG_RTC_MON
	/* Update all subscribed about RTC set */
	wake_up_all(&rtc_update_head);
	pr_info("rtc_update_head\n");
#endif

	return 0;
}

static int pm860x_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct pm860x_rtc_info *info = dev_get_drvdata(dev);
	unsigned char buf[8];
	unsigned long ticks, base, data;
	int ret;

	pm860x_page_bulk_read(info->i2c, REG0_ADDR, 8, buf);
	dev_dbg(info->dev, "%x-%x-%x-%x-%x-%x-%x-%x\n", buf[0], buf[1],
		buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	base = (buf[1] << 24) | (buf[3] << 16) | (buf[5] << 8) | buf[7];

	pm860x_bulk_read(info->i2c, PM8607_RTC_EXPIRE1, 4, buf);
	data = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
	ticks = base + data;
	dev_dbg(info->dev, "get base:0x%lx, RO count:0x%lx, ticks:0x%lx\n",
		base, data, ticks);

	rtc_time_to_tm(ticks, &alrm->time);
	ret = pm860x_reg_read(info->i2c, PM8607_RTC1);
	alrm->enabled = (ret & ALARM_EN) ? 1 : 0;
	alrm->pending = (ret & (ALARM | ALARM_WAKEUP)) ? 1 : 0;
	return 0;
}

static int pm860x_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct pm860x_rtc_info *info = dev_get_drvdata(dev);
	struct rtc_time now_tm, alarm_tm;
	unsigned long ticks, base, data;
	unsigned char buf[8];
	int mask;

	pm860x_set_bits(info->i2c, PM8607_RTC1, ALARM_EN, 0);

	pm860x_page_bulk_read(info->i2c, REG0_ADDR, 8, buf);
	dev_dbg(info->dev, "%x-%x-%x-%x-%x-%x-%x-%x\n", buf[0], buf[1],
		buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	base = (buf[1] << 24) | (buf[3] << 16) | (buf[5] << 8) | buf[7];

	/* load 32-bit read-only counter */
	pm860x_bulk_read(info->i2c, PM8607_RTC_COUNTER1, 4, buf);
	data = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
	ticks = base + data;
	dev_dbg(info->dev, "get base:0x%lx, RO count:0x%lx, ticks:0x%lx\n",
		base, data, ticks);

	rtc_time_to_tm(ticks, &now_tm);
	dev_dbg(info->dev, "%s, now time : %lu\n", __func__, ticks);
	rtc_next_alarm_time(&alarm_tm, &now_tm, &alrm->time);
	/* get new ticks for alarm in 24 hours */
	rtc_tm_to_time(&alarm_tm, &ticks);
	dev_dbg(info->dev, "%s, alarm time: %lu\n", __func__, ticks);
	data = ticks - base;

	buf[0] = data & 0xff;
	buf[1] = (data >> 8) & 0xff;
	buf[2] = (data >> 16) & 0xff;
	buf[3] = (data >> 24) & 0xff;
	pm860x_bulk_write(info->i2c, PM8607_RTC_EXPIRE1, 4, buf);
	if(alrm->enabled){
		mask = ALARM | ALARM_WAKEUP | ALARM_EN;
		pm860x_set_bits(info->i2c, PM8607_RTC1, mask, mask);
	} else {
		mask = ALARM | ALARM_WAKEUP | ALARM_EN;
		pm860x_set_bits(info->i2c, PM8607_RTC1, mask, ALARM | ALARM_WAKEUP);
	}
	return 0;
}

static const struct rtc_class_ops pm860x_rtc_ops = {
	.read_time	= pm860x_rtc_read_time,
	.set_time	= pm860x_rtc_set_time,
	.read_alarm	= pm860x_rtc_read_alarm,
	.set_alarm	= pm860x_rtc_set_alarm,
	.alarm_irq_enable = pm860x_rtc_alarm_irq_enable,
	.update_irq_enable = pm860x_rtc_alarm_irq_update,
};

#ifdef CONFIG_RTC_MON
static const struct file_operations pm860x_rtcmon_fops = {
	.owner				= THIS_MODULE,
	.open				= pm860x_rtcmon_open,
	.release			= pm860x_rtcmon_release,
	.unlocked_ioctl		= pm860x_rtcmon_ioctl,
};
static struct miscdevice rtcmon_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= "rtcmon",
	.fops		= &pm860x_rtcmon_fops,
};
#endif

#ifdef VRTC_CALIBRATION
static void calibrate_vrtc_work(struct work_struct *work)
{
	struct pm860x_rtc_info *info = container_of(work,
		struct pm860x_rtc_info, calib_work.work);
	unsigned char buf[2];
	unsigned int sum, data, mean, vrtc_set;
	int i, ret, tries = 0;

	for (i = 0, sum = 0; i < 16; i++) {
		msleep(100);

		do
			ret = pm860x_bulk_read(
					info->i2c, PM8607_VRTC_MEAS1, 2, buf);
		while ((ret < 0) && ((tries++) < VRTC_CALIB_TRIES));

		if (tries == VRTC_CALIB_TRIES)
			panic("%s: failed to read VRTC_MEAS1\n", __FUNCTION__);

		data = (buf[0] << 4) | buf[1];
		data = (data * 5400) >> 12;	/* convert to mv */
		sum += data;
		tries = 0;
	}
	mean = sum >> 4;
	vrtc_set = 2700 + (info->vrtc & 0x3) * 200;
	dev_dbg(info->dev, "mean:%d, vrtc_set:%d\n", mean, vrtc_set);

	sum = pm860x_reg_read(info->i2c, PM8607_RTC_MISC1);
	data = sum & 0x3;
	if ((mean + 200) < vrtc_set) {
		/* try higher voltage */
		if (++data == 4)
			goto out;
		data = (sum & 0xf8) | (data & 0x3);
		pm860x_reg_write(info->i2c, PM8607_RTC_MISC1, data);
	} else if ((mean - 200) > vrtc_set) {
		/* try lower voltage */
		if (data-- == 0)
			goto out;
		data = (sum & 0xf8) | (data & 0x3);
		pm860x_reg_write(info->i2c, PM8607_RTC_MISC1, data);
	} else
		goto out;
	dev_dbg(info->dev, "set 0x%x to RTC_MISC1\n", data);
	/* trigger next calibration since VRTC is updated */
	queue_delayed_work(info->chip->monitor_wqueue, &info->calib_work,
			   VRTC_CALIB_INTERVAL);
	return;
out:
	/* disable measurement */
	pm860x_set_bits(info->i2c, PM8607_MEAS_EN2, MEAS2_VRTC, 0);
	dev_dbg(info->dev, "finish VRTC calibration\n");
	return;
}
#endif

#ifdef CONFIG_PM
static int pm860x_rtc_suspend(struct device *dev)
{
	struct pm860x_rtc_info *info = dev_get_drvdata(dev);

	if (device_may_wakeup(dev)) {
		enable_irq_wake(info->chip->core_irq);
		enable_irq_wake(info->irq);
	}
	return 0;
}

static int pm860x_rtc_resume(struct device *dev)
{
	struct pm860x_rtc_info *info = dev_get_drvdata(dev);

	if (device_may_wakeup(dev)) {
		disable_irq_wake(info->chip->core_irq);
		disable_irq_wake(info->irq);
	}
	return 0;
}

static struct dev_pm_ops pm860x_rtc_pm_ops = {
	.suspend	= pm860x_rtc_suspend,
	.resume		= pm860x_rtc_resume,
};
#endif

static int __devinit pm860x_rtc_probe(struct platform_device *pdev)
{
	struct pm860x_chip *chip = dev_get_drvdata(pdev->dev.parent);
	struct pm860x_platform_data *pm860x_pdata;
	struct pm860x_rtc_pdata *pdata = NULL;
	struct pm860x_rtc_info *info;
	struct rtc_time tm;
	int ret;
	unsigned long ticks = 0;

	g_pdev = pdev;

#if defined(SYNC_TIME_TO_SOC) && defined(CONFIG_ARCH_MMP)
	rtc_base = (unsigned int *)ioremap(0xd4010000, 0x100);
	if (rtc_base == NULL) {
		pr_info("failed to remap RTC IO memory\n");
		return -EINVAL;
	}
#endif
	info = kzalloc(sizeof(struct pm860x_rtc_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;
	info->irq = platform_get_irq(pdev, 0);
	if (info->irq < 0) {
		dev_err(&pdev->dev, "No IRQ resource!\n");
		ret = -EINVAL;
		goto out;
	}

	info->chip = chip;
	info->i2c = (chip->id == CHIP_PM8607) ? chip->client : chip->companion;
	info->dev = &pdev->dev;
	dev_set_drvdata(&pdev->dev, info);

	ret = request_threaded_irq(info->irq, NULL, rtc_update_handler,
				   IRQF_ONESHOT, "rtc", info);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to request IRQ: #%d: %d\n",
			info->irq, ret);
		goto out;
	}

	/* set addresses of 32-bit base value for RTC time */
	pm860x_page_reg_write(info->i2c, REG0_ADDR, REG0_DATA);
	pm860x_page_reg_write(info->i2c, REG1_ADDR, REG1_DATA);
	pm860x_page_reg_write(info->i2c, REG2_ADDR, REG2_DATA);
	pm860x_page_reg_write(info->i2c, REG3_ADDR, REG3_DATA);

	ret = pm860x_rtc_read_time(&pdev->dev,&tm);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to read initial time.\n");
		goto out_rtc;
	}
	if((tm.tm_year <= 70) || (tm.tm_year > 138)) {
		tm.tm_year = 100;
		tm.tm_mon = 0;
		tm.tm_mday = 1;
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		ret = pm860x_rtc_set_time(&pdev->dev,&tm);
		if (ret < 0) {
			dev_err(&pdev->dev, "Failed to set initial time.\n");
			goto out_rtc;
		}
	}
#if defined(SYNC_TIME_TO_SOC)
	rtc_tm_to_time(&tm, &ticks);

#ifdef CONFIG_ARCH_MMP
	RCNR = ticks;
#elif defined(CONFIG_ARCH_PXA)
	pxa_rtc_sync_time(&tm);
#endif
	dev_dbg(info->dev, "%s, ticks:0x%lx, RCNR:0x%x\n",
			__func__, ticks, RCNR);
#endif

	info->rtc_dev = rtc_device_register("88pm860x-rtc", &pdev->dev,
					    &pm860x_rtc_ops, THIS_MODULE);
	ret = PTR_ERR(info->rtc_dev);
	if (IS_ERR(info->rtc_dev)) {
		dev_err(&pdev->dev, "Failed to register RTC device: %d\n", ret);
		goto out_rtc;
	}

#ifdef CONFIG_RTC_MON
	ret = misc_register(&rtcmon_miscdev);
	if	(ret < 0) {
		dev_err(&pdev->dev, "Failed to register rtcmon: %d\n", ret);
		goto out_rtc;
	} else
		pr_info("CONFIG_RTC_MON is on\n");
#endif


	/*
	 * enable internal XO instead of internal 3.25MHz clock since it can
	 * free running in PMIC power-down state.
	 */
	pm860x_set_bits(info->i2c, PM8607_RTC1, RTC1_USE_XO, RTC1_USE_XO);

#ifdef VRTC_CALIBRATION
	info->vrtc = 1;			/* By default, VRTC is 2.9V */
	if (pdev->dev.parent->platform_data) {
		pm860x_pdata = pdev->dev.parent->platform_data;
		pdata = pm860x_pdata->rtc;
		if (pdata)
			info->vrtc = pdata->vrtc & 0x3;
	}
	pm860x_set_bits(info->i2c, PM8607_MEAS_EN2, MEAS2_VRTC, MEAS2_VRTC);

	/* calibrate VRTC */
	INIT_DELAYED_WORK(&info->calib_work, calibrate_vrtc_work);
	queue_delayed_work(chip->monitor_wqueue, &info->calib_work,
			   VRTC_CALIB_INTERVAL);
#endif	/* VRTC_CALIBRATION */

	if (pdev->dev.parent->platform_data) {
		pm860x_pdata = pdev->dev.parent->platform_data;
		pdata = pm860x_pdata->rtc;
		if (pdata)
			info->rtc_dev->dev.platform_data = &pdata->rtc_wakeup;
	}
	device_init_wakeup(&pdev->dev, 1);
	return 0;
out_rtc:
	free_irq(info->irq, info);
out:
	kfree(info);
	return ret;
}

static int __devexit pm860x_rtc_remove(struct platform_device *pdev)
{
	struct pm860x_rtc_info *info = platform_get_drvdata(pdev);

#ifdef VRTC_CALIBRATION
	flush_workqueue(info->chip->monitor_wqueue);
	/* disable measurement */
	pm860x_set_bits(info->i2c, PM8607_MEAS_EN2, MEAS2_VRTC, 0);
#endif	/* VRTC_CALIBRATION */

	platform_set_drvdata(pdev, NULL);
	rtc_device_unregister(info->rtc_dev);
	free_irq(info->irq, info);
	kfree(info);
#if defined(SYNC_TIME_TO_SOC) && defined(CONFIG_ARCH_MMP)
	iounmap(rtc_base);
#endif
	return 0;
}

static struct platform_driver pm860x_rtc_driver = {
	.driver		= {
		.name	= "88pm860x-rtc",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &pm860x_rtc_pm_ops,
#endif
	},
	.probe		= pm860x_rtc_probe,
	.remove		= __devexit_p(pm860x_rtc_remove),
};

static int __init pm860x_rtc_init(void)
{
	return platform_driver_register(&pm860x_rtc_driver);
}
module_init(pm860x_rtc_init);

static void __exit pm860x_rtc_exit(void)
{
	platform_driver_unregister(&pm860x_rtc_driver);
#ifdef CONFIG_RTC_MON
	misc_deregister(&rtcmon_miscdev);
#endif
}
module_exit(pm860x_rtc_exit);

MODULE_DESCRIPTION("Marvell 88PM860x RTC driver");
MODULE_AUTHOR("Haojian Zhuang <haojian.zhuang@marvell.com>");
MODULE_LICENSE("GPL");
