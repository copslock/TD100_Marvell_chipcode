/*
 * This file is part of the ROHM BH1770GLC / OSRAM SFH7770 sensor driver.
 * Chip is combined proximity and ambient light sensor.
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Samu Onkalo <samu.p.onk...@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.         See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <linux/sfh7773.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/regulator/consumer.h>
#include "sfh7773.h"

struct input_dev *this_input_dev_als = NULL;

/* Supported stand alone rates in ms from chip data sheet */
static s16 als_rates[] = {100, 200, 500, 1000, 2000};

/* chip->mutex must be locked during this function */
static int bh1770glc_als_interrupt_control(struct bh1770glc_chip *chip,
                                          int als)
{
       chip->int_mode_als = als;

       /* Set ALS interrupt mode, interrupt active low, latched */
       return i2c_smbus_write_byte_data(chip->client,
                                       BHCHIP_INTERRUPT,
                                       (als << 1) | (chip->int_mode_ps << 0));
}

int bh1770glc_als_mode(struct bh1770glc_chip *chip, int mode)
{
       int r;

       r = i2c_smbus_write_byte_data(chip->client, BHCHIP_ALS_CONTROL, mode);

       if (r == 0)
               chip->als_mode = mode;

       return r;
}

static int bh1770glc_als_rate(struct bh1770glc_chip *chip, int rate)
{
       int ret = -EINVAL;
       int i;

       for (i = 0; i < ARRAY_SIZE(als_rates); i++)
               if (als_rates[i] == rate) {
                       ret = i2c_smbus_write_byte_data(chip->client,
                                                       BHCHIP_ALS_MEAS_RATE,
                                                       i);
                       if (ret == 0)
                               chip->als_rate = rate;
                       break;
               }
       return ret;
}

static int bh1770glc_als_set_threshold(struct bh1770glc_chip *chip,
                               int threshold_hi,
                               int threshold_lo)
{
       u8 data[4];
       int ret;

       chip->als_threshold_hi = threshold_hi;
       chip->als_threshold_lo = threshold_lo;

       data[0] = threshold_hi;
       data[1] = threshold_hi >> 8;
       data[2] = threshold_lo;
       data[3] = threshold_lo >> 8;

       ret = i2c_smbus_write_i2c_block_data(chip->client,
                                       BHCHIP_ALS_TH_UP_0,
                                       ARRAY_SIZE(data),
                                       data);
       return ret;
}

static int bh1770glc_als_calc_thresholds(struct bh1770glc_chip *chip, u16 data)
{
       int scaler;
       int hi_thres;
       int lo_thres;
       int sens;
       int ret;

       /*
        * Recalculate new threshold limits to simulate delta measurement
        * mode. New limits are relative to latest measurement data.
        */
       scaler = ((int)data * ALS_COEF) / ALS_NBR_FORMAT + ALS_ZERO_LEVEL;
       sens = chip->als_sens * scaler / ALS_NBR_FORMAT;

       hi_thres = min(data + sens, BHCHIP_ALS_RANGE);
       lo_thres = max(data - sens, 0);

       mutex_lock(&chip->mutex);
       ret = bh1770glc_als_set_threshold(chip,
                                       hi_thres,
                                       lo_thres);
       mutex_unlock(&chip->mutex);

       return ret;
}

static int bh1770glc_als_read_result(struct bh1770glc_chip *chip)
{
       u16 data;
       int ret;

       ret = i2c_smbus_read_byte_data(chip->client, BHCHIP_ALS_DATA_0);
       if (ret < 0)
               goto exit;

       data = ret & 0xff;
       ret = i2c_smbus_read_byte_data(chip->client, BHCHIP_ALS_DATA_1);
       if (ret < 0)
               goto exit;

       data = data | ((ret & 0xff) << 8);
       chip->als_data = data;
       chip->als_offset += sizeof(struct bh1770glc_als);

       ret = bh1770glc_als_calc_thresholds(chip, data);
exit:
       return ret;
}

static void bh1770glc_als_work_func(struct work_struct *work)
{
	unsigned long delay = atomic_read(&bh1770glc->delay);
	struct bh1770glc_chip *chip = container_of(work, struct bh1770glc_chip, als_work.work);
/*    int val = 0;*/

	bh1770glc_als_read_result(chip);
/*       printk("work als result [%d].\n",  chip->als_data);*/
	input_report_abs(chip->input_dev_als, ABS_PRESSURE, chip->als_data);
	input_sync(chip->input_dev_als);

	schedule_delayed_work(&bh1770glc->als_work, delay);
}

void bh1770glc_als_interrupt_handler(struct bh1770glc_chip *chip, int status)
{
		int result;
       if (chip->int_mode_als)
               if (status & BHCHIP_INT_ALS_INT) {
					
/*       printk("enter als work interrupt1.\n");*/
                   result =  bh1770glc_als_read_result(chip);
/*       printk("als result [%d].\n",  chip->als_data);*/
		
/*                       wake_up_interruptible(&bh1770glc->als_misc_wait);*/
               }
}
#if 0
static ssize_t bh1770glc_als_read(struct file *file, char __user *buf,
                       size_t count, loff_t *offset)
{
       struct bh1770glc_als als;
       struct bh1770glc_chip *chip = bh1770glc;
       u32 lux;

       if (count < sizeof(als))
               return -EINVAL;

       if (*offset >= chip->als_offset) {
               if (file->f_flags & O_NONBLOCK)
                       return -EAGAIN;
               if (wait_event_interruptible(bh1770glc->als_misc_wait,
                                               (*offset < chip->als_offset)))
                       return -ERESTARTSYS;
       }
       lux = ((u32)chip->als_data * chip->als_calib) /
               BHCHIP_CALIB_SCALER;
       lux = min(lux, (u32)BHCHIP_ALS_RANGE);

       als.lux = lux;

       *offset = chip->als_offset;

       return copy_to_user(buf, &als, sizeof(als)) ? -EFAULT : sizeof(als);
}
#endif
static int bh1770glc_als_open(void)
{

       struct bh1770glc_chip *chip = bh1770glc;
       int ret = 0;

       mutex_lock(&chip->mutex);
       if (!chip->als_users) {
#if 0
               ret = regulator_bulk_enable(ARRAY_SIZE(chip->regs),
                                       chip->regs);
#endif
           if (ret < 0)
                       goto release_lock;

               if (!chip->ps_users)
                       msleep(BHCHIP_STARTUP_DELAY);

               ret = bh1770glc_als_mode(chip, BHCHIP_STANDALONE);
               if (ret < 0)
                       goto exit;

               ret = bh1770glc_als_rate(chip, chip->als_rate);
               if (ret < 0)
                       goto exit;
#if 0
               ret = bh1770glc_als_interrupt_control(chip, BHCHIP_ENABLE);
               if (ret < 0)
                       goto exit;
#endif
       }
       /* Trig measurement and refresh the measurement result */
       bh1770glc_als_set_threshold(chip, BHCHIP_ALS_DEF_THRES,
                               BHCHIP_ALS_DEF_THRES);

       if (ret == 0)
               chip->als_users++;

exit:
       mutex_unlock(&chip->mutex);
#if 0
       if (ret < 0)
               regulator_bulk_disable(ARRAY_SIZE(chip->regs), chip->regs);
#endif
release_lock:
		if(ret == 0){
			   cancel_delayed_work_sync(&chip->als_work);
			   schedule_delayed_work(&chip->als_work,
							   msecs_to_jiffies(BHCHIP_PS_INIT_DELAY));
		}
/*       file->f_pos = chip->als_offset;*/
       /* In case of two or more user, provide newest results available */
/*       if (chip->als_users > 1 &&*/
/*           file->f_pos >= sizeof(struct bh1770glc_als))*/
/*               file->f_pos -= sizeof(struct bh1770glc_als);*/

       return ret;
}
#if 0
static unsigned int bh1770glc_als_poll(struct file *file, poll_table *wait)
{
       poll_wait(file, &bh1770glc->als_misc_wait, wait);
       if (file->f_pos < bh1770glc->als_offset)
               return POLLIN | POLLRDNORM;
       return 0;
}
#endif
static int bh1770glc_als_close(void)
{
       struct bh1770glc_chip *chip = bh1770glc;
       mutex_lock(&chip->mutex);
       if (!--chip->als_users) {
               bh1770glc_als_interrupt_control(chip, BHCHIP_DISABLE);
               bh1770glc_als_mode(chip, BHCHIP_STANDBY);
			   cancel_delayed_work_sync(&chip->als_work);
#if 0
               regulator_bulk_disable(ARRAY_SIZE(chip->regs), chip->regs);
#endif
    }
       mutex_unlock(&chip->mutex);
       return 0;
}


/* SYSFS interface */
static ssize_t bh1770glc_als_calib_show(struct device *dev,
                                struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip = dev_get_drvdata(dev);

       return snprintf(buf, PAGE_SIZE, "%u\n", chip->als_calib);
}

static ssize_t bh1770glc_als_calib_store(struct device *dev,
                                 struct device_attribute *attr,
                                 const char *buf, size_t len)
{
       struct bh1770glc_chip *chip = dev_get_drvdata(dev);
       unsigned long value;

       if (strict_strtoul(buf, 0, &value))
               return -EINVAL;

       chip->als_calib = value;

       return len;
}

static ssize_t bh1770glc_get_als_mode(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       return sprintf(buf, "%d\n", chip->als_mode);
}

static ssize_t bh1770glc_get_als_rate(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       return sprintf(buf, "%d\n", chip->als_rate);
}

static ssize_t bh1770glc_set_als_rate(struct device *dev,
                                     struct device_attribute *attr,
                                     const char *buf, size_t count)
{
#if 0
	printk("set als rate1.\n");
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
	printk("set als rate2.\n");
       unsigned long rate;
       int ret;

	printk("set als rate3.\n");
       if (strict_strtoul(buf, 0, &rate))
               return -EINVAL;

	printk("set als rate4.\n");
       mutex_lock(&chip->mutex);
	printk("set als rate5.\n");
       ret = bh1770glc_als_rate(chip, rate);
	printk("set als rate6.\n");
       mutex_unlock(&chip->mutex);
	printk("set als rate7.\n");

       if (ret < 0)
               return ret;
#endif
       return count;
}

static ssize_t bh1770glc_get_als_sens(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       return sprintf(buf, "%d\n", chip->als_sens);
}

static ssize_t bh1770glc_set_als_sens(struct device *dev,
                                     struct device_attribute *attr,
                                     const char *buf, size_t count)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       int ret;
       unsigned long sens;

       if (strict_strtoul(buf, 0, &sens))
               return -EINVAL;

       chip->als_sens = sens;

       /* Trick measurement by setting default thresholds */
       mutex_lock(&chip->mutex);
       ret = bh1770glc_als_set_threshold(chip,
                                       BHCHIP_ALS_DEF_THRES,
                                       BHCHIP_ALS_DEF_THRES);

       mutex_unlock(&chip->mutex);
       if (ret < 0)
               return ret;
       return count;
}

static ssize_t bh1770glc_show_active(struct device *dev, struct device_attribute *attr, char *buf)
{
	   struct input_dev *input = to_input_dev(dev);
	   struct bh1770glc_chip *chip = input_get_drvdata(input);

		return sprintf(buf, "%d\n", atomic_read(&chip->enable_als));
  
}

static void bh1770glc_set_enable(struct device *dev, unsigned long enable)
{
	   struct input_dev *input = to_input_dev(dev);
	   struct bh1770glc_chip *chip = input_get_drvdata(input);


       if(enable){
			if(!atomic_cmpxchg(&chip->enable_als, 0 , 1))
             bh1770glc_als_open();
}
       else{
			if(atomic_cmpxchg(&chip->enable_als, 1 , 0))
             bh1770glc_als_close();
}

	   atomic_set(&chip->enable_als, enable);


		printk("als enable %d.\n", enable);

}

static ssize_t bh1770glc_store_active(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{

	unsigned long enable = simple_strtoul(buf, NULL, 10);

    bh1770glc_set_enable(dev, enable);

    return count;
}

static DEVICE_ATTR(active, S_IRUGO | S_IWUGO, bh1770glc_show_active,
                                                bh1770glc_store_active);
static DEVICE_ATTR(als_mode, S_IRUGO , bh1770glc_get_als_mode, NULL);
static DEVICE_ATTR(interval, S_IRUGO | S_IWUGO, bh1770glc_get_als_rate,
                                               bh1770glc_set_als_rate);
static DEVICE_ATTR(als_sens, S_IRUGO | S_IWUGO, bh1770glc_get_als_sens,
                                               bh1770glc_set_als_sens);
#if 0
static struct attribute *sysfs_attrs[] = {
       &dev_attr_als_calib.attr,
       &dev_attr_als_mode.attr,
       &dev_attr_als_rate.attr,
       &dev_attr_als_sens.attr,
       NULL
};
#endif

static struct attribute *sysfs_attrs[] = {
	&dev_attr_active.attr,
	&dev_attr_interval.attr,
/*    &dev_attr_als_mode.attr,*/
/*    &dev_attr_wake.attr,*/
/*    &dev_attr_data.attr,*/
/*    &dev_attr_status.attr,*/
	NULL
};

static struct attribute_group bh1770glc_attribute_group = {
       .attrs = sysfs_attrs
};
#if 0
static const struct file_operations bh1770glc_als_fops = {
       .owner          = THIS_MODULE,
       .llseek         = no_llseek,
       .read           = bh1770glc_als_read,
       .poll           = bh1770glc_als_poll,
       .open           = bh1770glc_als_open,
       .release        = bh1770glc_als_close,
};

static struct miscdevice bh1770glc_als_miscdevice = {
       .minor = MISC_DYNAMIC_MINOR,
       .name  = "bh1770glc_als",
       .fops  = &bh1770glc_als_fops
};
#endif
int bh1770glc_als_init(struct bh1770glc_chip *chip)
{
       int err;
	   struct input_dev *input_dev_als = NULL;
       err = bh1770glc_als_mode(chip, BHCHIP_STANDBY);
       if (err < 0)
               goto fail;

       err = bh1770glc_als_interrupt_control(chip, BHCHIP_DISABLE);
       if (err < 0)
               goto fail;

       chip->als_rate = BHCHIP_ALS_DEFAULT_RATE;

     
       INIT_DELAYED_WORK(&chip->als_work, bh1770glc_als_work_func);
       input_dev_als = input_allocate_device();
       if (!input_dev_als ) {
		err = -ENOMEM;
		printk(KERN_ERR "bh1770glc_init: Failed to allocate input_data device\n");
		goto fail;
	}

      input_dev_als->name = "sfh7773 light sensor";
	  input_dev_als->id.bustype = BUS_I2C;
	input_set_capability(input_dev_als, EV_ABS, ABS_MISC);
	__set_bit(EV_ABS, input_dev_als->evbit);
	__set_bit(ABS_PRESSURE, input_dev_als->absbit);
	input_set_abs_params(input_dev_als, ABS_PRESSURE, -100000, 100000, 0, 0);
	input_set_drvdata(input_dev_als, chip);

       err = input_register_device(input_dev_als);
	if (err < 0) {
		input_free_device(input_dev_als);
		goto fail2;
	}
	
	chip->input_dev_als = input_dev_als;

       err = sysfs_create_group(&input_dev_als->dev.kobj,
                               &bh1770glc_attribute_group);


       if (err < 0) {
               dev_err(&input_dev_als->dev, "Sysfs registration failed\n");
               goto fail2;
       }
	   this_input_dev_als = input_dev_als;

       return 0;
fail2:
        input_unregister_device(input_dev_als);
		input_free_device(input_dev_als);
       //misc_deregister(&bh1770glc_als_miscdevice);
fail:
       return err;
}

int bh1770glc_als_destroy(struct bh1770glc_chip *chip)
{
	
	if(this_input_dev_als != NULL){
	   chip = input_get_drvdata(this_input_dev_als);
       cancel_delayed_work_sync(&chip->als_work);
       sysfs_remove_group(&this_input_dev_als->dev.kobj,
                       &bh1770glc_attribute_group);
	
	
        input_unregister_device(this_input_dev_als);

		if(chip != NULL)
			kfree(chip);

}

/*       misc_deregister(&bh1770glc_als_miscdevice);*/
       return 0;
}
