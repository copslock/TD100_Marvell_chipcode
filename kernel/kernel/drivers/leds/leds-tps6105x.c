/*
 * LED driver for Marvell 88PM860x
 *
 * Copyright (C) 2009 Marvell International Ltd.
 *	Haojian Zhuang <haojian.zhuang@marvell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include <plat/mfp.h>
#include <mach/gpio.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/mfd/tps61050.h>

#define MFD_NAME_SIZE	32

struct tps6105x_led {
	struct led_classdev cdev;
	struct i2c_client *i2c;
	struct work_struct work;
	struct device	*master;
	struct tps6105x *tps6105x;
	struct mutex lock;
	char name[MFD_NAME_SIZE];

	unsigned char brightness;
	unsigned char current_brightness;

};

static int tps6105x_led_shutdown(struct tps6105x *pdev);
static int __devinit tps6105x_led_prepare(struct tps6105x *pdev);
static int __devinit tps6105x_led_set_torch_brightness(struct tps6105x *pdev);
static int __devinit tps6105x_led_set_brightness(struct tps6105x *pdev);


static void tps6105x_led_work(struct work_struct *work)
{

	struct tps6105x_led *led;
	int flash_sync;

	flash_sync = mfp_to_gpio(63);

	led = container_of(work, struct tps6105x_led, work);
	mutex_lock(&led->lock);

	/* TO DO */
	if((led->current_brightness == 0) && led->brightness){
		tps6105x_led_set_torch_brightness(led->tps6105x);
/*        tps6105x_led_set_brightness(led->tps6105x);*/
/*        tps6105x_led_prepare(led->tps6105x);*/

	}else if(led->brightness == 0)
		tps6105x_led_shutdown(led->tps6105x);

	mutex_unlock(&led->lock);
}

static int tps6105x_led_shutdown(struct tps6105x *pdev)
{

	struct tps6105x *tps6105x = pdev;
	/*  shut down mode */
	tps6105x_set(pdev, TPS6105X_REG_0, 0x0);
	tps6105x_set(pdev, TPS6105X_REG_1, 0x0);
	tps6105x_set(pdev, TPS6105X_REG_2, 0x0);/* enable Tx-MASK  */
	tps6105x_set(pdev, TPS6105X_REG_3, 0x0);

	return 0;
}

static int __devinit tps6105x_led_set_torch_brightness(struct tps6105x *pdev)
{
	int ret = 0;
	u8 regval;
	struct tps6105x *tps6105x = pdev;

#if 1
	/*  only torch mode, set torch current 200mA, flash current 500mA, disabled Tx-MASK, Level sensitive timer. */
	ret = tps6105x_set(pdev, TPS6105X_REG_0, 0x55);
	ret = tps6105x_set(pdev, TPS6105X_REG_1, 0x44);
	ret = tps6105x_set(pdev, TPS6105X_REG_2, 0x01);/* disable Tx-MASK  */
	ret = tps6105x_set(pdev, TPS6105X_REG_3, 0xD1);/* STIM 557.6 ms, DCTIM 6.3% x ITORCH */
#else
/*  only torch mode, set torch current 500mA, flash current 500mA, enabled Tx-MASK, Level sensitive timer. */
	ret = tps6105x_set(pdev, TPS6105X_REG_0, 0x47);
	ret = tps6105x_set(pdev, TPS6105X_REG_1, 0x44);
	ret = tps6105x_set(pdev, TPS6105X_REG_2, 0x06);
	ret = tps6105x_set(pdev, TPS6105X_REG_3, 0xD1);/* STIM 557.6 ms, DCTIM 6.3% x ITORCH */

#endif

	return ret;
}


static int __devinit tps6105x_led_set_brightness(struct tps6105x *pdev)
{
	int ret = 0;
	u8 regval;
	int flash_sync;
	struct tps6105x *tps6105x = pdev;

	printk("tps6105x set brightness .\n");

	flash_sync = mfp_to_gpio(63);

	if(gpio_request(flash_sync, "flash sync")){
		printk(KERN_ERR "Request GPIO %d failed.\n", flash_sync);
		return -EIO;
	}

	/*  torch+flash mode, set torch current 0mA, flash current 900mA, enabled Tx-MASK, Level sensitive timer. */
	ret = tps6105x_set(pdev, TPS6105X_REG_0, 0x90);
	ret = tps6105x_set(pdev, TPS6105X_REG_1, 0x86);/* 500mA 0x84 */
	ret = tps6105x_set(pdev, TPS6105X_REG_2, 0x05);/* enable Tx-MASK  */
	ret = tps6105x_set(pdev, TPS6105X_REG_3, 0xD1);/* STIM 557.6 ms, DCTIM 6.3% x ITORCH */

#if 1
	/* create one FLASH_SYNC event */
	gpio_direction_output(flash_sync, 0);
	mdelay(10);
	gpio_direction_output(flash_sync, 1);

#endif
	mdelay(560);

	gpio_direction_output(flash_sync, 0);

	gpio_free(flash_sync);

	return ret;
}

static int __devinit tps6105x_led_prepare(struct tps6105x *pdev)
{
	int ret = 0;
	u8 regval;
	int flash_sync;
	struct tps6105x *tps6105x = pdev;

	printk("tps6105x led prepare.\n");

	flash_sync = mfp_to_gpio(63);

	if(gpio_request(flash_sync, "flash sync")){
		printk(KERN_ERR "Request GPIO %d failed.\n", flash_sync);
		return -EIO;
	}

	/*  torch+flash mode, set torch current 0mA, flash current 500mA, enabled Tx-MASK, Level sensitive timer. */
	ret = tps6105x_set(pdev, TPS6105X_REG_0, 0x90);
	ret = tps6105x_set(pdev, TPS6105X_REG_1, 0x84);
	ret = tps6105x_set(pdev, TPS6105X_REG_2, 0x05);/* enable Tx-MASK  */
	ret = tps6105x_set(pdev, TPS6105X_REG_3, 0xD1);/* STIM 557.6 ms, DCTIM 6.3% x ITORCH */

#if 0
	/* create one FLASH_SYNC event */
	gpio_direction_output(flash_sync, 0);
	mdelay(10);
	gpio_direction_output(flash_sync, 1);

	gpio_direction_output(flash_sync, 0);
#endif
	mdelay(560);

	/*  shut down mode */
	tps6105x_set(pdev, TPS6105X_REG_0, 0x0);
	tps6105x_set(pdev, TPS6105X_REG_1, 0x0);
	tps6105x_set(pdev, TPS6105X_REG_2, 0x0);
	tps6105x_set(pdev, TPS6105X_REG_3, 0x0);

	printk("tps6105x led prepare finish.\n");

	gpio_free(flash_sync);

	return ret;
}

static void tps6105x_led_set(struct led_classdev *cdev,
			   enum led_brightness value)
{
	struct tps6105x_led *data = container_of(cdev, struct tps6105x_led, cdev);

	data->brightness = value;
	schedule_work(&data->work);
}


/*
 * sysfs device attributes
 */
static ssize_t tps6105x_flash_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	return 0;
}

static ssize_t tps6105x_flash_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	return 0;
}

static DEVICE_ATTR(flash123, S_IRUGO | S_IWUGO,
		   tps6105x_flash_show, tps6105x_flash_store);

static struct attribute *tps6105x_attributes[] = {
	&dev_attr_flash123.attr,
	NULL
};

static struct attribute_group tps6105x_attribute_group = {
	.attrs = tps6105x_attributes
};



static int tps6105x_led_probe(struct platform_device *pdev)
{
	struct tps6105x_platform_data *tps6105x_pdata;
	struct tps6105x_led *data;
	struct tps6105x *tps6105x = dev_get_platdata(&pdev->dev);
	int ret;

	tps6105x_pdata = tps6105x->pdata;

	printk("tps6105x_led_probe.\n");

	if(tps6105x->pdata->mode != TPS6105X_MODE_TORCH_FLASH){
		dev_info(&pdev->dev,
				"chip not in torch-flash mode, exit probe.\n");
		return 0;
	}

	data = kzalloc(sizeof(struct tps6105x_led), GFP_KERNEL);
	if (data == NULL)
		return -ENOMEM;
/*    strncpy(data->name, "tps6105x-leds", MFD_NAME_SIZE-1);*/
	strncpy(data->name, "spotlight", MFD_NAME_SIZE-1);
	platform_set_drvdata(pdev, tps6105x);

#if 0
	tps6105x_led_prepare(tps6105x);
#endif

	data->master = &tps6105x->client->dev;
	data->tps6105x = tps6105x;
	data->current_brightness = 0;
	data->cdev.name = data->name;
	data->cdev.brightness_set = tps6105x_led_set;
	mutex_init(&data->lock);
	INIT_WORK(&data->work, tps6105x_led_work);

	ret = led_classdev_register(data->master, &data->cdev);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register LED: %d\n", ret);
		goto out;
	}

/*    ret = device_create_file(data->master, &dev_attr_flash123);*/

#if 1
	ret = sysfs_create_group(&data->cdev.dev->kobj,
			       &tps6105x_attribute_group);
#endif

	tps6105x_led_shutdown(tps6105x);

	printk("tps6105x_led_probe finish.\n");

	return 0;
out:
	kfree(data);
	return ret;
}

static int tps6105x_led_remove(struct platform_device *pdev)
{
	struct tps6105x_led *data = platform_get_drvdata(pdev);

	led_classdev_unregister(&data->cdev);
	kfree(data);

	return 0;
}

static struct platform_driver tps6105x_led_driver = {
	.driver	= {
/*        .name	= "tps6105x-leds",*/
		.name	= "spotlight",
		.owner	= THIS_MODULE,
	},
	.probe	= tps6105x_led_probe,
	.remove	= tps6105x_led_remove,
};

static int __devinit tps6105x_led_init(void)
{
	printk("tps6105x led init.\n");
	return platform_driver_register(&tps6105x_led_driver);
}
module_init(tps6105x_led_init);

static void __devexit tps6105x_led_exit(void)
{
	platform_driver_unregister(&tps6105x_led_driver);
}
module_exit(tps6105x_led_exit);

MODULE_DESCRIPTION("LED driver for TI tps6105x");
MODULE_AUTHOR("Li Yan <li.yanl@dbjtech.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:tps6105x-led");
