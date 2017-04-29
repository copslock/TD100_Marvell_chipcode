/* Copyright (C) 2010 Marvell */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/mfd/88pm860x.h>
#include <linux/slab.h>

#include "../staging/android/timed_output.h"

struct vibrator_info {
	struct pm860x_chip	*chip;
	struct i2c_client	*i2c;
	struct timed_output_dev vibrator_timed_dev;
	struct timer_list	vibrate_timer;
	struct work_struct	vibrator_off_work;
	unsigned char vibratorA;/*PM8606_VIBRATORA register offset*/
	unsigned char vibratorB;/*PM8606_VIBRATORB register offset*/
};

#define VIBRA_OFF_VALUE	0
#define VIBRA_ON_VALUE	1

int pm860x_control_vibrator(struct pm860x_chip *chip, struct i2c_client *i2c,
		unsigned char value)
{
	if (value == VIBRA_OFF_VALUE) {
		pm860x_reg_write(i2c, PM8606_VIBRATORA,0x00);
		pm8606_ref_gp_and_osc_release(chip, LDO_VBR_EN);
		pm860x_reg_write(i2c, PM8606_VIBRATORB,0xFF);
	} else if (value == VIBRA_ON_VALUE) {
		pm8606_ref_gp_and_osc_get(chip, LDO_VBR_EN);
		pm860x_reg_write(i2c, PM8606_VIBRATORA,0x09);
		pm860x_reg_write(i2c, PM8606_VIBRATORB,0xFF);
	}
	return 0;
}

static void vibrator_off_worker(struct work_struct *work)
{
	struct vibrator_info *info;
	info = container_of(work, struct vibrator_info, vibrator_off_work);
	pm860x_control_vibrator(info->chip, info->i2c, 0);
}

static void on_vibrate_timer_expired(unsigned long x)
{
	struct vibrator_info *info;
	info = (struct vibrator_info*)x;
	schedule_work(&info->vibrator_off_work);
}

static void vibrator_enable_set_timeout(struct timed_output_dev *sdev,
	int timeout)
{
	struct vibrator_info *info;
	info = container_of(sdev, struct vibrator_info, vibrator_timed_dev);
	pr_debug("Vibrator: Set duration: %dms\n", timeout);
	pm860x_control_vibrator(info->chip, info->i2c, 1);
	mod_timer(&info->vibrate_timer, jiffies + msecs_to_jiffies(timeout));
	return;
}

static int vibrator_get_remaining_time(struct timed_output_dev *sdev)
{
	struct vibrator_info *info;
	int retTime;
	info = container_of(sdev, struct vibrator_info, vibrator_timed_dev);
	retTime = jiffies_to_msecs(jiffies-info->vibrate_timer.expires);
	pr_debug("Vibrator: Current duration: %dms\n", retTime);
	return retTime;
}

static int vibrator_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct resource *res;
	struct pm860x_chip *chip = dev_get_drvdata(pdev->dev.parent);
	struct vibrator_info *info = kzalloc(sizeof(struct vibrator_info), GFP_KERNEL);
	if (!info)
			return -ENOMEM;
	res = platform_get_resource(pdev, IORESOURCE_IO, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "No I/O resource!\n");
		kfree(info);
		return -EINVAL;
	}
	info->vibratorA = res->start;
	info->vibratorB = res->end;
	info->chip = chip;
	info->i2c = (chip->id == CHIP_PM8606) ? chip->client : chip->companion;

	/* Setup timed_output obj */
	info->vibrator_timed_dev.name = "vibrator";
	info->vibrator_timed_dev.enable = vibrator_enable_set_timeout;
	info->vibrator_timed_dev.get_time = vibrator_get_remaining_time;
	/* Vibrator dev register in /sys/class/timed_output/ */
	ret = timed_output_dev_register(&info->vibrator_timed_dev);
	if (ret < 0) {
		printk(KERN_ERR "Vibrator: timed_output dev registration failure\n");
		timed_output_dev_unregister(&info->vibrator_timed_dev);
	}

	INIT_WORK(&info->vibrator_off_work, vibrator_off_worker);

	init_timer(&info->vibrate_timer);
	info->vibrate_timer.function = on_vibrate_timer_expired;
	info->vibrate_timer.data = (unsigned long)info;

	platform_set_drvdata(pdev, info);

	return 0;
}

static int __devexit vibrator_remove(struct platform_device *pdev)
{
	struct vibrator_info *info;
	info = platform_get_drvdata(pdev);
	timed_output_dev_unregister(&info->vibrator_timed_dev);
	return 0;
}

static struct platform_driver vibrator_driver = {
	.probe		= vibrator_probe,
	.remove		= __devexit_p(vibrator_remove),
	.driver		= {
		.name	= "android-vibrator",
		.owner	= THIS_MODULE,
	},
};

static int __init vibrator_init(void)
{
	return platform_driver_register(&vibrator_driver);
}

static void __exit vibrator_exit(void)
{
	platform_driver_unregister(&vibrator_driver);
}

module_init(vibrator_init);
module_exit(vibrator_exit);

MODULE_DESCRIPTION("Android Vibrator driver");
MODULE_LICENSE("GPL");
