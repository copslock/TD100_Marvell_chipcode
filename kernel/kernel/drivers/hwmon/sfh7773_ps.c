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
#include <linux/regulator/consumer.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>
#include "sfh7773.h"

struct input_dev *this_input_dev_ps = NULL;

/* Supported stand alone rates in ms from chip data sheet */
static s16 ps_rates[] = {10, 20, 30, 40, 70, 100, 200, 500, 1000, 2000};

/* Supported IR-led currents in mA */
static const u8 ps_curr_ma[] = {5, 10, 20, 50, 100, 150, 200};

/* chip->mutex must be locked during this function */
static int bh1770glc_ps_interrupt_control(struct bh1770glc_chip *chip, int ps)
{
       chip->int_mode_ps = ps;
/*        printk("als %d, ps %d. \n", chip->int_mode_als, ps);*/

       /* Set PS interrupt mode, interrupt active low, latched */
       return i2c_smbus_write_byte_data(chip->client,
                                       BHCHIP_INTERRUPT,
									   (chip->int_mode_als << 1) | (ps << 0));
}

/* LEDs are controlled by the chip during proximity scanning */
static int bh1770glc_led_cfg(struct bh1770glc_chip *chip, u8 ledcurr[3])
{
       int ret;
/*Setting LED pulse current, only setting the LED*/
#if 0
       ledcfg = chip->pdata->leds;

       for (i = 0; i < BHCHIP_PS_CHANNELS; i++)
               if (ledcurr[i] > chip->pdata->led_max_curr)
                       return -EINVAL;

       for (i = 0; i < BHCHIP_PS_CHANNELS; i++)
               chip->ps_leds[i] = ledcurr[i];

#endif

       /* LED cfg, 200mA current for I_LED register 0x82  */
       ret = i2c_smbus_write_byte_data(chip->client,
                                       BHCHIP_I_LED,
                                       0x1E);

       if (ret < 0)
		{
				printk("Failed to set I_LED Current.\n");
               goto fail;

		}
#if 0
       /* Current for led 3*/
       ret = i2c_smbus_write_byte_data(chip->client,
                                       BHCHIP_I_LED3,
                                   ledcurr[2]);
#endif

fail:
       return ret;
}

int bh1770glc_ps_mode(struct bh1770glc_chip *chip, int mode)
{
       int ret;

       ret = i2c_smbus_write_byte_data(chip->client, BHCHIP_PS_CONTROL, mode);
       if (ret == 0)
               chip->ps_mode = mode;
       return ret;
}

int bh1770glc_ps_int_time(struct bh1770glc_chip *chip, int int_time)
{
       int ret;

       ret = i2c_smbus_write_byte_data(chip->client, BHCHIP_PS_INT_TIME, int_time);
       if (ret == 0)
              chip->ps_int_time = int_time;
       return ret;
}


static int bh1770glc_ps_rates(struct bh1770glc_chip *chip, int rate,
                       int rate_threshold)
{
       int i;

       for (i = 0; i < ARRAY_SIZE(ps_rates); i++) {
               if (ps_rates[i] == rate) {
                       chip->ps_rate = i;
                       break;
               }
       }
       if (i == ARRAY_SIZE(ps_rates))
               return -EINVAL;

       for (i = 0; i < ARRAY_SIZE(ps_rates); i++) {
               if (ps_rates[i] == rate_threshold) {
                       chip->ps_rate_threshold = i;
                       return 0;
               }
       }
       return -EINVAL;
}

static int bh1770glc_ps_rate(struct bh1770glc_chip *chip, int mode)
{
       int ret;
       int rate;

       rate = (mode == PS_ABOVE_THRESHOLD) ?
               chip->ps_rate_threshold : chip->ps_rate;

       ret = i2c_smbus_write_byte_data(chip->client,
                                       BHCHIP_PS_MEAS_RATE,
                                       rate);
       return ret;
}

static int bh1770glc_ps_read_result(struct bh1770glc_chip *chip)
{
       int ret;

       mutex_lock(&chip->mutex);
#if 0
       for (i = 0; i < ARRAY_SIZE(chip->ps_data); i++) {
#endif
               ret = i2c_smbus_read_byte_data(chip->client,
                                       BHCHIP_PS_DATA_LED1);
               if (ret < 0)
                       goto out;
               chip->ps_data[0] = ret;
/*                printk("ps_data %d.\n",chip->ps_data[0]);*/
#if 0
       }
       chip->ps_offset += sizeof(struct bh1770glc_ps);
#endif
out:
       mutex_unlock(&chip->mutex);
       return ret;
}

static int bh1770glc_ps_set_thresholds(struct bh1770glc_chip *chip)
{
       int ret, i;
       u8 data[BHCHIP_PS_CHANNELS];
       u32 tmp;

       for (i = 0; i < BHCHIP_PS_CHANNELS; i++) {
               tmp = ((u32)chip->ps_thresholds[i] * BHCHIP_CALIB_SCALER) /
                       chip->ps_calib[i];
               if (tmp > BHCHIP_PS_RANGE)
                       tmp = BHCHIP_PS_RANGE;
               data[i] = tmp;
       }

       ret = i2c_smbus_write_i2c_block_data(chip->client,
                                        BHCHIP_PS_TH_LED1,
                                        BHCHIP_PS_CHANNELS,
                                        data);
       return ret;
}

#if 0
static ssize_t bh1770glc_ps_read(struct file *file, char __user *buf,
                       size_t count, loff_t *offset)
{
       struct bh1770glc_ps ps;
       struct bh1770glc_chip *chip = bh1770glc;
       int i;
       u16 data[BHCHIP_PS_CHANNELS];

       if (count < sizeof(ps))
               return -EINVAL;

       if (*offset >= chip->ps_offset) {
               if (file->f_flags & O_NONBLOCK)
                       return -EAGAIN;
               if (wait_event_interruptible(bh1770glc->ps_misc_wait,
                                               (*offset < chip->ps_offset)))
                       return -ERESTARTSYS;
       }

       for (i = 0; i < BHCHIP_PS_CHANNELS; i++) {
               data[i] = ((u32)chip->ps_data[i] * chip->ps_calib[i]) /
                       BHCHIP_CALIB_SCALER;
               if (data[i] > BHCHIP_PS_RANGE)
                       data[i] = BHCHIP_PS_RANGE;
       }

       ps.led1 = data[0];
       ps.led2 = data[1];
       ps.led3 = data[2];

       *offset = chip->ps_offset;

       return copy_to_user(buf, &ps, sizeof(ps)) ? -EFAULT : sizeof(ps);
}
#endif
static void bh1770glc_ps_work(struct work_struct *work)
{
       struct bh1770glc_chip *chip 
	   						 = container_of(work, struct bh1770glc_chip, ps_work.work);

	unsigned long delay = atomic_read(&bh1770glc->delay);
	int val = 0;

       bh1770glc_ps_rate(chip, PS_BELOW_THRESHOLD);
       val = bh1770glc_ps_read_result(chip);
	   
	printk("enter ps work thread value [%d].\n", val);

	if(val < BHCHIP_PS_DEF_THRES) {
		/* report leaving the face value*/
		input_report_abs(chip->input_dev_ps, ABS_DISTANCE, 5);
		val = 1;
	} else {
		/* report closing to the face value */
		input_report_abs(chip->input_dev_ps, ABS_DISTANCE, 0);
		val = 0;
	}
	input_sync(chip->input_dev_ps);


	schedule_delayed_work(&bh1770glc->ps_work, delay);
	   

/*       wake_up_interruptible(&chip->ps_misc_wait);*/
}

void bh1770glc_ps_interrupt_handler(struct bh1770glc_chip *chip, int status)
{
	int rate;
	if (chip->int_mode_ps)
               if (status & BHCHIP_INT_LED1_INT) {
					   rate = ps_rates[chip->ps_rate_threshold];

                       bh1770glc_ps_read_result(chip);
                       bh1770glc_ps_rate(chip, PS_ABOVE_THRESHOLD);
/*                       wake_up_interruptible(&bh1770glc->ps_misc_wait);*/

                       cancel_delayed_work_sync(&chip->ps_work);
                       /*
                        * Let's recheck situation 50 ms after the next
                        * expected threshold interrupt.
                        */
                       schedule_delayed_work(&chip->ps_work,
                         msecs_to_jiffies(rate + 50));
               }
}

#if 0
/* Proximity misc device */
static unsigned int bh1770glc_ps_poll(struct file *file, poll_table *wait)
{
       poll_wait(file, &bh1770glc->ps_misc_wait, wait);
       if (file->f_pos < bh1770glc->ps_offset)
               return POLLIN | POLLRDNORM;
       return 0;
}
#endif
static int bh1770glc_ps_open(void)
{
       struct bh1770glc_chip *chip = bh1770glc;
       int err;

       mutex_lock(&chip->mutex);
       err = 0;
       if (!chip->ps_users) {
#if 0
               err = regulator_bulk_enable(ARRAY_SIZE(chip->regs),
                                       chip->regs);
          if (err < 0)
                       goto release_lock;
#endif
               if (!chip->als_users)
				{
						printk("als user equal zero.\n");
                       msleep(BHCHIP_STARTUP_DELAY);
				}
               /* Refresh all configs in case of regulators were off */
               err = bh1770glc_ps_set_thresholds(chip);
               if (err < 0)
                       goto exit;

               err = bh1770glc_led_cfg(chip, chip->ps_leds);
               if (err < 0)
                       goto exit;

               err = bh1770glc_ps_rate(chip, PS_BELOW_THRESHOLD);
			   if (err < 0)
				   goto exit;

#if 0
			   err = bh1770glc_ps_interrupt_control(chip, BHCHIP_ENABLE);                
			   if (err < 0)
				   goto exit;
#endif
			   err = bh1770glc_ps_mode(chip, BHCHIP_STANDALONE);

/*               err = bh1770glc_ps_int_time(chip, BHCHIP_PS_DEF_INT_TIME);*/
	   }
	   if (err == 0)
               chip->ps_users++;
exit:
#if 0
       if (err < 0)
               regulator_bulk_disable(ARRAY_SIZE(chip->regs), chip->regs);
release_lock:
#endif
       mutex_unlock(&chip->mutex);

       if (err == 0) {
			printk("begin schedule the ps work.\n");
			   cancel_delayed_work_sync(&chip->ps_work);
			   schedule_delayed_work(&chip->ps_work,
							   msecs_to_jiffies(BHCHIP_PS_INIT_DELAY));
       }

       return err;
}

#if 1
static int bh1770glc_ps_close(void)
{
       struct bh1770glc_chip *chip = bh1770glc;
       mutex_lock(&chip->mutex);
       if (!--chip->ps_users) {
               bh1770glc_ps_interrupt_control(chip, BHCHIP_DISABLE);
               bh1770glc_ps_mode(chip, BHCHIP_STANDBY);
			   cancel_delayed_work_sync(&chip->ps_work);
  }
       mutex_unlock(&chip->mutex);
       return 0;
}
#endif

static ssize_t bh1770glc_get_ps_mode(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       return sprintf(buf, "%d\n", chip->ps_mode);
}

static ssize_t bh1770glc_get_ps_rate(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       return sprintf(buf, "%d %d\n", ps_rates[chip->ps_rate],
               ps_rates[chip->ps_rate_threshold]);
}

static ssize_t bh1770glc_set_ps_rate(struct device *dev,
                                     struct device_attribute *attr,
                                     const char *buf, size_t count)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       int rate = 0, rate_threshold = 0;
       int ret;

       ret = sscanf(buf, "%d %d", &rate, &rate_threshold);
       if (ret < 0)
               return ret;

       if (ret == 0)
               return count;

       /* Second value is optional */
       if (ret == 1)
               rate_threshold = ps_rates[chip->ps_rate_threshold];

       mutex_lock(&chip->mutex);
       ret = bh1770glc_ps_rates(chip, rate, rate_threshold);
       mutex_unlock(&chip->mutex);

       if (ret < 0)
               return ret;

       return count;
}

static ssize_t bh1770glc_ps_calib_show(struct device *dev,
                                struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip = dev_get_drvdata(dev);

       return snprintf(buf, PAGE_SIZE, "%u %u %u\n", chip->ps_calib[0],
                       chip->ps_calib[1], chip->ps_calib[2]);
}

static ssize_t bh1770glc_ps_calib_store(struct device *dev,
                                 struct device_attribute *attr,
                                 const char *buf, size_t len)
{
       struct bh1770glc_chip *chip = dev_get_drvdata(dev);
       int calib[BHCHIP_PS_CHANNELS];
       int i, ret;

       for (i = 0; i < BHCHIP_PS_CHANNELS; i++)
               calib[i] = BHCHIP_PS_NEUTRAL_CALIB_VALUE;
       ret = sscanf(buf, "%d %d %d", &calib[0], &calib[1], &calib[2]);
       if (ret < 0)
               return ret;
       if (ret < chip->ps_channels)
               return -EINVAL;

       for (i = 0; i < chip->ps_channels; i++)
               chip->ps_calib[i] = calib[i];

       return len;
}

static ssize_t bh1770glc_get_ps_thres(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       return sprintf(buf, "%d %d %d\n", chip->ps_thresholds[0],
               chip->ps_thresholds[1],
               chip->ps_thresholds[2]);
}

static ssize_t bh1770glc_set_ps_thres(struct device *dev,
                                     struct device_attribute *attr,
                                     const char *buf, size_t count)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       int input[BHCHIP_PS_CHANNELS];
       int ret;
       int i;

       for (i = 0; i < ARRAY_SIZE(input); i++)
               input[i] = BHCHIP_PS_RANGE;
       ret = sscanf(buf, "%d %d %d", &input[0], &input[1], &input[2]);

       if (ret < 0)
               return ret;
       if (ret < chip->ps_channels)
               return -EINVAL;

       for (i = 0; i < ARRAY_SIZE(input); i++) {
               if ((input[i] < 0) ||
                   (input[i] > BHCHIP_PS_RANGE))
                       return -EINVAL;
               chip->ps_thresholds[i] = input[i];
       }

       mutex_lock(&chip->mutex);
       ret = bh1770glc_ps_set_thresholds(chip);
       mutex_unlock(&chip->mutex);
       if (ret < 0)
               return ret;

       return count;
}

static ssize_t bh1770glc_ps_leds_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       u8 led_current[BHCHIP_PS_CHANNELS];
       int i;

       memset(led_current, 0, sizeof(led_current));
       for (i = 0; i < chip->ps_channels; i++)
               led_current[i] = ps_curr_ma[chip->ps_leds[i]];

       return sprintf(buf, "%d %d %d\n", led_current[0],
               led_current[1],
               led_current[2]);
}

static ssize_t bh1770glc_ps_leds_store(struct device *dev,
                                     struct device_attribute *attr,
                                     const char *buf, size_t count)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       int input[BHCHIP_PS_CHANNELS];
       u8 led_curr[BHCHIP_PS_CHANNELS];
       int ret;
       int i, j;

       ret = sscanf(buf, "%d %d %d", &input[0], &input[1], &input[2]);
       if (ret < 0)
               return ret;
       if (ret < chip->ps_channels)
               return -EINVAL;

       /* Set minimum current */
       for (i = chip->ps_channels; i < BHCHIP_PS_CHANNELS; i++)
               led_curr[i] = BH1770GLC_LED_5mA;
        for (i = 0; i < chip->ps_channels; i++) {
               for (j = 0; j < ARRAY_SIZE(ps_curr_ma); j++)
                       if (input[i] == ps_curr_ma[j]) {
                               led_curr[i] = j;
                               break;
                       }
               if (j == ARRAY_SIZE(ps_curr_ma))
                       return -EINVAL;
       }

       mutex_lock(&chip->mutex);
       ret = bh1770glc_led_cfg(chip, led_curr);
       mutex_unlock(&chip->mutex);
       if (ret < 0)
               return ret;

       return count;
}

static ssize_t bh1770glc_chip_id_show(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
       struct bh1770glc_chip *chip =  dev_get_drvdata(dev);
       return sprintf(buf, "%s rev %d\n", chip->chipname, chip->revision);
}



static ssize_t bh1770glc_show_active(struct device *dev,
                                  struct device_attribute *attr, char *buf)
{
	
	   struct input_dev *input = to_input_dev(dev);
	   struct bh1770glc_chip *chip = input_get_drvdata(input);

		return sprintf(buf, "%d\n", atomic_read(&chip->enable_ps));
  
}

static void bh1770glc_set_enable(struct device *dev, unsigned long enable)
{	
	   struct input_dev *input = to_input_dev(dev);
	   struct bh1770glc_chip *chip = input_get_drvdata(input);

	   if(enable){
			if(!atomic_cmpxchg(&chip->enable_ps, 0 , 1))
            bh1770glc_ps_open();
}
	   else{
			if(atomic_cmpxchg(&chip->enable_ps, 1 , 0))
		    bh1770glc_ps_close();
}
	   atomic_set(&chip->enable_ps, enable);
	   
		printk("ps enable %d.\n", enable);
	
}

static ssize_t bh1770glc_store_active(struct device *dev, 
							struct device_attribute *attr,
							const char *buf, size_t count)
{	
	unsigned long enable = simple_strtoul(buf, NULL, 10);

	bh1770glc_set_enable(dev, enable);
    
	return count;
}


static DEVICE_ATTR(active, S_IWUGO | S_IRUGO, bh1770glc_show_active, bh1770glc_store_active);
static DEVICE_ATTR(ps_mode, S_IRUGO, bh1770glc_get_ps_mode, NULL);
static DEVICE_ATTR(interval, S_IRUGO | S_IWUGO, bh1770glc_get_ps_rate,
                                               bh1770glc_set_ps_rate);
static DEVICE_ATTR(ps_threshold, S_IRUGO | S_IWUSR, bh1770glc_get_ps_thres,
                                               bh1770glc_set_ps_thres);
static DEVICE_ATTR(ps_calib, S_IRUGO | S_IWUSR, bh1770glc_ps_calib_show,
                                                bh1770glc_ps_calib_store);
static DEVICE_ATTR(ps_leds, S_IRUGO | S_IWUSR, bh1770glc_ps_leds_show,
                                                bh1770glc_ps_leds_store);
static DEVICE_ATTR(chip_id, S_IRUGO, bh1770glc_chip_id_show, NULL);

static struct attribute *sysfs_attrs[] = {
	   &dev_attr_active.attr,
       &dev_attr_interval.attr,
       //&dev_attr_ps_mode.attr,
       //&dev_attr_ps_rate.attr,
       //&dev_attr_ps_threshold.attr,
       //&dev_attr_ps_leds.attr,
       //&dev_attr_chip_id.attr,
       NULL
};

static struct attribute_group bh1770glc_attribute_group = {
       .attrs = sysfs_attrs
};

#if 0
static const struct file_operations bh1770glc_ps_fops = {
       .owner          = THIS_MODULE,
       .llseek         = no_llseek,
       .read           = bh1770glc_ps_read,
       .poll           = bh1770glc_ps_poll,
       .open           = bh1770glc_ps_open,
       .release        = bh1770glc_ps_close,
};

static struct miscdevice bh1770glc_ps_miscdevice = {
       .minor = MISC_DYNAMIC_MINOR,
       .name = "bh1770glc_ps",
       .fops = &bh1770glc_ps_fops
};
#endif

int bh1770glc_ps_init(struct bh1770glc_chip *chip)
{
       int err;
	   struct input_dev *input_dev_ps = NULL;
       int i;

       for (i = 0; i < BHCHIP_PS_CHANNELS; i++) {
               chip->ps_thresholds[i] = BHCHIP_PS_DEF_THRES;
               chip->ps_leds[i] = chip->pdata->led_def_curr[i];
       }

       err = bh1770glc_ps_mode(chip, BHCHIP_STANDBY);
       if (err < 0)
               goto fail1;

       err = bh1770glc_ps_interrupt_control(chip, BHCHIP_DISABLE);
       if (err < 0)
               goto fail1;

       bh1770glc_ps_rates(chip, BHCHIP_PS_DEFAULT_RATE,
                       BHCHIP_PS_DEF_RATE_THRESH);


       INIT_DELAYED_WORK(&chip->ps_work, bh1770glc_ps_work);

#if 0
	   bh1770glc_ps_open();
#endif
	   input_dev_ps = input_allocate_device();

		   if(!input_dev_ps){
			   err = -ENOMEM;
			   printk("Failed to allocate input device ps.\n");
			   goto fail1;
		   }

	   set_bit(EV_ABS, input_dev_ps->evbit);

	   input_set_abs_params(input_dev_ps, ABS_DISTANCE, 0, 1, 0, 0);

	   input_dev_ps->name = "sfh7773 proximity sensor";

	input_set_drvdata(input_dev_ps, chip);

	   err = input_register_device(input_dev_ps);

	   if(err){
		   err = -ENOMEM;
		   printk("Unable to register input device ps.\n");
		   goto fail2;
	   }

	   chip->input_dev_ps = input_dev_ps;

	   err = sysfs_create_group(&input_dev_ps->dev.kobj,
			   &bh1770glc_attribute_group);
	   if (err < 0) {
		   dev_err(&input_dev_ps->dev, "Sysfs registration failed\n");
               goto fail3;
       }

	   this_input_dev_ps = input_dev_ps;

       return 0;
fail3:
       input_unregister_device(input_dev_ps);
fail2:
       input_free_device(input_dev_ps);
fail1:
       return err;

}

int bh1770glc_ps_destroy(struct bh1770glc_chip *chip)
{
	if(this_input_dev_ps != NULL){
	   chip = input_get_drvdata(this_input_dev_ps);
       cancel_delayed_work_sync(&chip->ps_work);
       sysfs_remove_group(&this_input_dev_ps->dev.kobj,
                       &bh1770glc_attribute_group);
	
	
        input_unregister_device(this_input_dev_ps);

		if(chip != NULL)
			kfree(chip);

}




/*       misc_deregister(&bh1770glc_ps_miscdevice);*/
       return 0;
}
