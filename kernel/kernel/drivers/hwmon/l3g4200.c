/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
*
* File Name		: l3g4200d_gyr_sysfs.c
* Authors		: MH - C&I BU - Application Team
*			: Carmine Iascone (carmine.iascone@st.com)
*			: Matteo Dameno (matteo.dameno@st.com)
* Version		: V 1.0 sysfs
* Date			: 19/11/2010
* Description		: L3G4200D digital output gyroscope sensor API
*
********************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* THE PRESENT SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES
* OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, FOR THE SOLE
* PURPOSE TO SUPPORT YOUR APPLICATION DEVELOPMENT.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
********************************************************************************
* REVISON HISTORY
*
* VERSION | DATE 	| AUTHORS	     | DESCRIPTION
*
* 1.0	  | 19/11/2010	| Carmine Iascone    | First Release
*
*******************************************************************************/

#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/input-polldev.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/input.h>
#include <linux/kernel.h>

#include <linux/i2c/l3g4200d.h>


//#define DEV_NAME		"l3g4200d_gyr"

/** Maximum polled-device-reported rot speed value value in dps*/
#define FS_MAX			32768

/* l3g4200d gyroscope registers */
#define WHO_AM_I        0x0F

#define CTRL_REG1       0x20    /* CTRL REG1 */
#define CTRL_REG2       0x21    /* CTRL REG2 */
#define CTRL_REG3       0x22    /* CTRL_REG3 */
#define CTRL_REG4       0x23    /* CTRL_REG4 */
#define CTRL_REG5       0x24    /* CTRL_REG5 */

/* CTRL_REG1 */
#define PM_OFF		0x00
#define PM_NORMAL	0x08
#define ENABLE_ALL_AXES	0x07
#define BW00		0x00
#define BW01		0x10
#define BW10		0x20
#define BW11		0x30
#define ODR100		0x00  /* ODR = 100Hz */
#define ODR200		0x40  /* ODR = 200Hz */
#define ODR400		0x80  /* ODR = 400Hz */
#define ODR800		0xC0  /* ODR = 800Hz */


#define AXISDATA_REG    0x28

#define FUZZ			0
#define FLAT			0
#define AUTO_INCREMENT		0x80

/*#define SENSITIVITY_250DPS	8.75
#define SENSITIVITY_500DPS	17.50
#define SENSITIVITY_2000DPS	70*/

#define DEBUG 1

/** Registers Contents */
#define WHOAMI_L3G4200D		0x00D3	/* Expected content for WAI register*/

/*
 * Default parameters
 */
#define L3G4200D_DEFAULT_DELAY         100
#define L3G4200D_MAX_DELAY             2000

/*
 * L3G4200D gyroscope data
 * brief structure containing gyroscope values for yaw, pitch and roll in
 * signed short
 */

struct l3g4200d_t {
	short	x,	/* x-axis angular rate data. */
		y,	/* y-axis angluar rate data. */
		z;	/* z-axis angular rate data. */
};

struct output_rate {
	int poll_rate_ms;
	u8 mask;
};

static const struct output_rate odr_table[] = {

	{	2,	ODR800|BW10},
	{	3,	ODR400|BW01},
	{	5,	ODR200|BW00},
	{	10,	ODR100|BW00},
};

struct l3g4200d_data {
	struct i2c_client *client;
	struct l3g4200d_gyr_platform_data *pdata;

	struct mutex lock;

	struct input_dev *input;
	struct delayed_work work;
	struct input_polled_dev *input_poll_dev;
	int hw_initialized;
	atomic_t enabled;
	atomic_t delay;		/* attribute value */
	u8 reg_addr;
	u8 resume_state[5];
};

static int l3g4200d_i2c_read(struct l3g4200d_data *gyro,
				  u8 *buf, int len)
{
	int err;

	struct i2c_msg msgs[] = {
		{
		 .addr = gyro->client->addr,
		 .flags = gyro->client->flags & I2C_M_TEN,
		 .len = 1,
		 .buf = buf,
		 },
		{
		 .addr = gyro->client->addr,
		 .flags = (gyro->client->flags & I2C_M_TEN) | I2C_M_RD,
		 .len = len,
		 .buf = buf,
		 },
	};

	err = i2c_transfer(gyro->client->adapter, msgs, 2);

	if (err != 2) {
		dev_err(&gyro->client->dev, "read transfer error\n");
		err = -EIO;
	} else {
		err = 0;
	}

	return err;
}

static int l3g4200d_i2c_write(struct l3g4200d_data *gyro,
						u8 *buf,
						u8 len)
{
	int err;

	struct i2c_msg msgs[] = {
		{
		 .addr = gyro->client->addr,
		 .flags = gyro->client->flags & I2C_M_TEN,
		 .len = len + 1,
		 .buf = buf,
		 },
	};

	err = i2c_transfer(gyro->client->adapter, msgs, 1);

	if (err != 1) {
		dev_err(&gyro->client->dev, "write transfer error\n");
		return -EIO;
	}

	return 0;
}

static int l3g4200d_update_fs_range(struct l3g4200d_data *gyro, u8 new_fs)
{
	int err = -1;
	int res = -1;
	u8 buf[2];

	if (atomic_read(&gyro->enabled)) {

		buf[0] = CTRL_REG4;

		res = l3g4200d_i2c_read(gyro, buf, 1);

		if (res >= 0)
			buf[0] = buf[0] & 0x00CF;
		else
			return res;

		buf[1] = new_fs|buf[0];
		buf[0] = CTRL_REG4;
		err = l3g4200d_i2c_write(gyro, buf, 1);
		if (err < 0)
			return err;
	}

	gyro->resume_state[3] = new_fs;

	return err;
}

static int l3g4200d_update_odr(struct l3g4200d_data *gyro,
				int poll_interval)
{
	int err = -1;
	int i;
	u8 config[2];

	for (i = ARRAY_SIZE(odr_table) - 1; i >= 0; i--) {
		if (odr_table[i].poll_rate_ms <= poll_interval)
			break;
	}

	config[1] = odr_table[i].mask;
	config[1] |= (ENABLE_ALL_AXES + PM_NORMAL);

	/* If device is currently enabled, we need to write new
	 *  configuration out to it */
	if (atomic_read(&gyro->enabled)) {
		config[0] = CTRL_REG1;
		err = l3g4200d_i2c_write(gyro, config, 1);
		if (err < 0)
			return err;
	}

	gyro->resume_state[0] = config[1];

	return 0;
}

/* gyroscope data readout */
static int l3g4200d_get_data(struct l3g4200d_data *gyro,
			     struct l3g4200d_t *data)
{
	int err = -1;
	unsigned char gyro_out[6];
	/* y,p,r hardware data */
	s16 hw_d[3] = { 0 };

	gyro_out[0] = (AUTO_INCREMENT | AXISDATA_REG);

	err = l3g4200d_i2c_read(gyro, gyro_out, 6);

	if (err < 0)
		return err;

	hw_d[0] = (s16) (((gyro_out[1]) << 8) | gyro_out[0]);
	hw_d[1] = (s16) (((gyro_out[3]) << 8) | gyro_out[2]);
	hw_d[2] = (s16) (((gyro_out[5]) << 8) | gyro_out[4]);

	data->x = ((gyro->pdata->negate_x) ? (-hw_d[gyro->pdata->axis_map_x])
		   : (hw_d[gyro->pdata->axis_map_x]));
	data->y = ((gyro->pdata->negate_y) ? (-hw_d[gyro->pdata->axis_map_y])
		   : (hw_d[gyro->pdata->axis_map_y]));
	data->z = ((gyro->pdata->negate_z) ? (-hw_d[gyro->pdata->axis_map_z])
		   : (hw_d[gyro->pdata->axis_map_z]));

	#if DEBUG
	 printk(KERN_INFO "gyro_out: y = %d p = %d r= %d\n",
		data->x, data->y, data->z);
	#endif

	return err;
}

static void l3g4200d_report_values(struct l3g4200d_data *l3g,
						struct l3g4200d_t *data)
{
	struct input_dev *input = l3g->input_poll_dev->input;
	input_report_abs(input, ABS_X, data->x);
	input_report_abs(input, ABS_Y, data->y);
	input_report_abs(input, ABS_Z, data->z);
	input_sync(input);
}

static void l3g4200d_work_func(struct work_struct *work)
{
	struct l3g4200d_data *l3g4200d =
	    container_of((struct delayed_work *)work,
			 struct l3g4200d_data, work);
	unsigned long delay = atomic_read(&l3g4200d->delay);
	struct l3g4200d_t data;

	data.x = data.y = data.z = 0;

	l3g4200d_get_data(l3g4200d, &data);

	input_report_abs(l3g4200d->input, ABS_X, data.x);
	input_report_abs(l3g4200d->input, ABS_Y, data.y);
	input_report_abs(l3g4200d->input, ABS_Z, data.z);
	input_sync(l3g4200d->input);

	schedule_delayed_work(&l3g4200d->work, delay);
}

static int l3g4200d_hw_init(struct l3g4200d_data *gyro)
{
	int err = -1;
	u8 buf[6];

	printk(KERN_INFO "%s hw init\n", L3G4200D_DEV_NAME);

	buf[0] = (AUTO_INCREMENT | CTRL_REG1);
	buf[1] = gyro->resume_state[0];
	buf[2] = gyro->resume_state[1];
	buf[3] = gyro->resume_state[2];
	buf[4] = gyro->resume_state[3];
	buf[5] = gyro->resume_state[4];

	err = l3g4200d_i2c_write(gyro, buf, 5);

	if (err < 0)
		return err;

	gyro->hw_initialized = 1;

	return 0;
}

static void l3g4200d_device_power_off(struct l3g4200d_data *dev_data)
{
	int err;
	u8 buf[2];

	pr_info("%s power off\n", L3G4200D_DEV_NAME);

	buf[0] = CTRL_REG1;
	buf[1] = PM_OFF;
	err = l3g4200d_i2c_write(dev_data, buf, 1);
	if (err < 0)
		dev_err(&dev_data->client->dev, "soft power off failed\n");

	if (dev_data->pdata->power_off) {
		dev_data->pdata->power_off();
		dev_data->hw_initialized = 0;
	}

	if (dev_data->hw_initialized)
		dev_data->hw_initialized = 0;

}

static int l3g4200d_device_power_on(struct l3g4200d_data *dev_data)
{
	int err;

	if (dev_data->pdata->power_on) {
		err = dev_data->pdata->power_on();
		if (err < 0)
			return err;
	}


	if (!dev_data->hw_initialized) {
		err = l3g4200d_hw_init(dev_data);
		if (err < 0) {
			l3g4200d_device_power_off(dev_data);
			return err;
		}
	}

	return 0;
}

static int l3g4200d_enable(struct l3g4200d_data *dev_data)
{
	int err;
	int delay = 0;

	delay = atomic_read(&dev_data->delay);

	if (!atomic_cmpxchg(&dev_data->enabled, 0, 1)) {

		err = l3g4200d_device_power_on(dev_data);
		if (err < 0) {
			atomic_set(&dev_data->enabled, 0);
			return err;
		}
		atomic_set(&dev_data->enabled, 1);
		schedule_delayed_work(&dev_data->work,
				msecs_to_jiffies(delay) + 1);
	}

	return 0;
}

static int l3g4200d_disable(struct l3g4200d_data *dev_data)
{
	if (atomic_cmpxchg(&dev_data->enabled, 1, 0))
		l3g4200d_device_power_off(dev_data);

	atomic_set(&dev_data->enabled, 0);
	cancel_delayed_work_sync(&dev_data->work);

	return 0;
}

static ssize_t attr_get_polling_rate(struct device *dev,
				     struct device_attribute *attr,
				     char *buf)
{
	int val;
/*    struct l3g4200d_data *gyro = dev_get_drvdata(dev);*/
	struct input_dev *input = to_input_dev(dev);
	struct l3g4200d_data *gyro = input_get_drvdata(input);
	mutex_lock(&gyro->lock);
	val = gyro->pdata->poll_interval;
	mutex_unlock(&gyro->lock);
	return sprintf(buf, "%d\n", val);
}

static ssize_t attr_set_polling_rate(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t size)
{
/*    struct l3g4200d_data *gyro = dev_get_drvdata(dev);*/
	struct input_dev *input = to_input_dev(dev);
	struct l3g4200d_data *gyro = input_get_drvdata(input);
	unsigned long interval_ms;

	if (strict_strtoul(buf, 10, &interval_ms))
		return -EINVAL;
	if (!interval_ms)
		return -EINVAL;
	mutex_lock(&gyro->lock);
	gyro->pdata->poll_interval = interval_ms;
	l3g4200d_update_odr(gyro, interval_ms);
	mutex_unlock(&gyro->lock);
	return size;
}

static void l3g4200d_set_delay(struct l3g4200d_data *l3g4200d, unsigned long delay)
{

	if (delay < l3g4200d->pdata->min_interval) {
		printk(
			"delay value %d ms is too small, set to min delay %d ms\n",
			delay, l3g4200d->pdata->min_interval);
		delay = l3g4200d->pdata->min_interval;
	}

	atomic_set(&l3g4200d->delay, delay);

	mutex_lock(&l3g4200d->lock);

	/* update CTRL register and reschedule work_queue if enable=1 */
	l3g4200d_update_odr(l3g4200d, delay);

	mutex_unlock(&l3g4200d->lock);
}

static ssize_t attr_get_range(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	struct l3g4200d_data *gyro = dev_get_drvdata(dev);
	int range = 0;
	char val;
	mutex_lock(&gyro->lock);
	val = gyro->pdata->fs_range;
	switch (val) {
	case L3G4200D_FS_250DPS:
		range = 250;
		break;
	case L3G4200D_FS_500DPS:
		range = 500;
		break;
	case L3G4200D_FS_2000DPS:
		range = 2000;
		break;
	}
	mutex_unlock(&gyro->lock);
	return sprintf(buf, "%d\n", range);
}

static ssize_t attr_set_range(struct device *dev,
			      struct device_attribute *attr,
			      const char *buf, size_t size)
{
	struct l3g4200d_data *gyro = dev_get_drvdata(dev);
	unsigned long val;
	if (strict_strtoul(buf, 10, &val))
		return -EINVAL;
	mutex_lock(&gyro->lock);
	gyro->pdata->fs_range = val;
	l3g4200d_update_fs_range(gyro, val);
	mutex_unlock(&gyro->lock);
	return size;
}

static ssize_t attr_get_enable(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
/*    struct l3g4200d_data *gyro = dev_get_drvdata(dev);*/
	struct input_dev *input = to_input_dev(dev);
	struct l3g4200d_data *gyro = input_get_drvdata(input);
	int val = atomic_read(&gyro->enabled);
	return sprintf(buf, "%d\n", val);
}

static ssize_t attr_set_enable(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t size)
{
/*    struct l3g4200d_data *gyro = dev_get_drvdata(dev);*/
	struct input_dev *input = to_input_dev(dev);
	struct l3g4200d_data *gyro = input_get_drvdata(input);
	unsigned long val;

	if (strict_strtoul(buf, 10, &val))
		return -EINVAL;

	if(gyro == NULL)
		printk("enale gyro is null.\n");
#if 1
	if (val)
		l3g4200d_enable(gyro);
	else
		l3g4200d_disable(gyro);
#endif
	return size;
}

static ssize_t attr_reg_set(struct device *dev, struct device_attribute *attr,
				const char *buf, size_t size)
{
	int rc;
	struct l3g4200d_data *gyro = dev_get_drvdata(dev);
	u8 x[2];
	unsigned long val;

	if (strict_strtoul(buf, 16, &val))
		return -EINVAL;
	mutex_lock(&gyro->lock);
	x[0] = gyro->reg_addr;
	mutex_unlock(&gyro->lock);
	x[1] = val;
	rc = l3g4200d_i2c_write(gyro, x, 1);
	return size;
}

static ssize_t attr_reg_get(struct device *dev, struct device_attribute *attr,
				char *buf)
{
	ssize_t ret;
	struct l3g4200d_data *gyro = dev_get_drvdata(dev);
	int rc;
	u8 data;

	mutex_lock(&gyro->lock);
	data = gyro->reg_addr;
	mutex_unlock(&gyro->lock);
	rc = l3g4200d_i2c_read(gyro, &data, 1);
	ret = sprintf(buf, "0x%02x\n", data);
	return ret;
}

static ssize_t attr_addr_set(struct device *dev, struct device_attribute *attr,
				const char *buf, size_t size)
{
	struct l3g4200d_data *gyro = dev_get_drvdata(dev);
	unsigned long val;

	if (strict_strtoul(buf, 16, &val))
		return -EINVAL;

	mutex_lock(&gyro->lock);

	gyro->reg_addr = val;

	mutex_unlock(&gyro->lock);

	return size;
}

static DEVICE_ATTR(active, S_IRUGO | S_IWUGO,
		   attr_get_enable, attr_set_enable);
static DEVICE_ATTR(interval, S_IRUGO | S_IWUGO,
		   attr_get_polling_rate, attr_set_polling_rate);


static struct attribute *l3g4200d_attributes[] = {
	&dev_attr_active.attr,
	&dev_attr_interval.attr,
	NULL
};

static struct attribute_group l3g4200d_attribute_group = {
	.attrs = l3g4200d_attributes
};

static struct device_attribute attributes[] = {
	__ATTR(pollrate_ms, 0666, attr_get_polling_rate, attr_set_polling_rate),
	__ATTR(range, 0666, attr_get_range, attr_set_range),
	__ATTR(enable, 0666, attr_get_enable, attr_set_enable),
	__ATTR(reg_value, 0600, attr_reg_get, attr_reg_set),
	__ATTR(reg_addr, 0200, NULL, attr_addr_set),
};

static int create_sysfs_interfaces(struct device *dev)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(attributes); i++)
		if (device_create_file(dev, attributes + i))
			goto error;
	return 0;

error:
	for ( ; i >= 0; i--)
		device_remove_file(dev, attributes + i);
	dev_err(dev, "%s:Unable to create interface\n", __func__);
	return -1;
}

static int remove_sysfs_interfaces(struct device *dev)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(attributes); i++)
		device_remove_file(dev, attributes + i);
	return 0;
}

static void l3g4200d_input_poll_func(struct input_polled_dev *dev)
{
	struct l3g4200d_data *gyro = dev->private;

	struct l3g4200d_t data_out;

	int err;

	/* dev_data = container_of((struct delayed_work *)work,
				 struct l3g4200d_data, input_work); */

	mutex_lock(&gyro->lock);
	err = l3g4200d_get_data(gyro, &data_out);
	if (err < 0)
		dev_err(&gyro->client->dev, "get_gyroscope_data failed\n");
	else
		l3g4200d_report_values(gyro, &data_out);

	mutex_unlock(&gyro->lock);

}

int l3g4200d_input_open(struct input_dev *input)
{
	struct l3g4200d_data *gyro = input_get_drvdata(input);

	return l3g4200d_enable(gyro);
}

void l3g4200d_input_close(struct input_dev *dev)
{
	struct l3g4200d_data *gyro = input_get_drvdata(dev);

	l3g4200d_disable(gyro);
}

static int l3g4200d_validate_pdata(struct l3g4200d_data *gyro)
{
	gyro->pdata->poll_interval = max(gyro->pdata->poll_interval,
			gyro->pdata->min_interval);

	if (gyro->pdata->axis_map_x > 2 ||
	    gyro->pdata->axis_map_y > 2 ||
	    gyro->pdata->axis_map_z > 2) {
		dev_err(&gyro->client->dev,
			"invalid axis_map value x:%u y:%u z%u\n",
			gyro->pdata->axis_map_x,
			gyro->pdata->axis_map_y,
			gyro->pdata->axis_map_z);
		return -EINVAL;
	}

	/* Only allow 0 and 1 for negation boolean flag */
	if (gyro->pdata->negate_x > 1 ||
	    gyro->pdata->negate_y > 1 ||
	    gyro->pdata->negate_z > 1) {
		dev_err(&gyro->client->dev,
			"invalid negate value x:%u y:%u z:%u\n",
			gyro->pdata->negate_x,
			gyro->pdata->negate_y,
			gyro->pdata->negate_z);
		return -EINVAL;
	}

	/* Enforce minimum polling interval */
	if (gyro->pdata->poll_interval < gyro->pdata->min_interval) {
		dev_err(&gyro->client->dev,
			"minimum poll interval violated\n");
		return -EINVAL;
	}
	return 0;
}

#if 1
static int l3g4200d_input_init(struct l3g4200d_data *gyro)
{
	int err = -1;
	struct input_dev *input_data;


	input_data = input_allocate_device();
	if (!input_data) {
		err = -ENOMEM;
		dev_err(&gyro->client->dev,
			"input device allocate failed\n");
		goto err0;
	}

	input_data->id.bustype = BUS_I2C;
	input_data->name = L3G4200D_DEV_NAME;
	input_data->dev.parent = &gyro->client->dev;


	set_bit(EV_ABS, input_data->evbit);

	input_set_abs_params(input_data, ABS_X, -FS_MAX, FS_MAX, FUZZ, FLAT);
	input_set_abs_params(input_data, ABS_Y, -FS_MAX, FS_MAX, FUZZ, FLAT);
	input_set_abs_params(input_data, ABS_Z, -FS_MAX, FS_MAX, FUZZ, FLAT);

	input_set_drvdata(input_data, gyro);


	err = input_register_device(input_data);
	if (err < 0) {
		input_free_device(input_data);
		goto err1;
	}
	gyro->input = input_data;

	return 0;

err1:
	input_free_device(gyro->input);
err0:
	return err;

}
#else
static int l3g4200d_input_init(struct l3g4200d_data *gyro)
{
	int err = -1;
	struct input_dev *input;


	gyro->input_poll_dev = input_allocate_polled_device();
	if (!gyro->input_poll_dev) {
		err = -ENOMEM;
		dev_err(&gyro->client->dev,
			"input device allocate failed\n");
		goto err0;
	}

	gyro->input_poll_dev->private = gyro;
	gyro->input_poll_dev->poll = l3g4200d_input_poll_func;
	gyro->input_poll_dev->poll_interval = gyro->pdata->poll_interval;

	input = gyro->input_poll_dev->input;

	input->open = l3g4200d_input_open;
	input->close = l3g4200d_input_close;

	input->id.bustype = BUS_I2C;
	input->dev.parent = &gyro->client->dev;

	input_set_drvdata(gyro->input_poll_dev->input, gyro);

	set_bit(EV_ABS, input->evbit);

	input_set_abs_params(input, ABS_X, -FS_MAX, FS_MAX, FUZZ, FLAT);
	input_set_abs_params(input, ABS_Y, -FS_MAX, FS_MAX, FUZZ, FLAT);
	input_set_abs_params(input, ABS_Z, -FS_MAX, FS_MAX, FUZZ, FLAT);

	input->name = L3G4200D_DEV_NAME;

	err = input_register_polled_device(gyro->input_poll_dev);
	if (err) {
		dev_err(&gyro->client->dev,
			"unable to register input polled device %s\n",
			gyro->input_poll_dev->input->name);
		goto err1;
	}

	return 0;

err1:
	input_free_polled_device(gyro->input_poll_dev);
err0:
	return err;
}
#endif

static void l3g4200d_input_cleanup(struct l3g4200d_data *gyro)
{
	input_unregister_polled_device(gyro->input_poll_dev);
	input_free_polled_device(gyro->input_poll_dev);
}

static int l3g4200d_probe(struct i2c_client *client,
					const struct i2c_device_id *devid)
{
	struct l3g4200d_data *gyro;

	int err = -1;

	pr_err("%s: probe start.\n", L3G4200D_DEV_NAME);

	if (client->dev.platform_data == NULL) {
		dev_err(&client->dev, "platform data is NULL. exiting.\n");
		err = -ENODEV;
		goto err0;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "client not i2c capable:1\n");
		err = -ENODEV;
		goto err0;
	}

	gyro = kzalloc(sizeof(*gyro), GFP_KERNEL);
	if (gyro == NULL) {
		dev_err(&client->dev,
			"failed to allocate memory for module data\n");
		err = -ENOMEM;
		goto err0;
	}

	mutex_init(&gyro->lock);
	mutex_lock(&gyro->lock);
	gyro->client = client;
	i2c_set_clientdata(client, gyro);

	gyro->pdata = kmalloc(sizeof(*gyro->pdata), GFP_KERNEL);
	if (gyro->pdata == NULL) {
		dev_err(&client->dev,
			"failed to allocate memory for pdata: %d\n", err);
		goto err1;
	}
	memcpy(gyro->pdata, client->dev.platform_data,
						sizeof(*gyro->pdata));

	err = l3g4200d_validate_pdata(gyro);
	if (err < 0) {
		dev_err(&client->dev, "failed to validate platform data\n");
		goto err1_1;
	}


	if (gyro->pdata->init) {
		err = gyro->pdata->init();
		if (err < 0) {
			dev_err(&client->dev, "init failed: %d\n", err);
			goto err1_1;
		}
	}

	atomic_set(&gyro->delay, L3G4200D_DEFAULT_DELAY);

	/* setup driver interfaces */
	INIT_DELAYED_WORK(&gyro->work, l3g4200d_work_func);

	memset(gyro->resume_state, 0, ARRAY_SIZE(gyro->resume_state));

	gyro->resume_state[0] = 0x07;
	gyro->resume_state[1] = 0x00;
	gyro->resume_state[2] = 0x00;
	gyro->resume_state[3] = 0x00;
	gyro->resume_state[4] = 0x00;

	err = l3g4200d_device_power_on(gyro);
	if (err < 0) {
		dev_err(&client->dev, "power on failed: %d\n", err);
		goto err2;
	}

	atomic_set(&gyro->enabled, 1);

	err = l3g4200d_update_fs_range(gyro, gyro->pdata->fs_range);
	if (err < 0) {
		dev_err(&client->dev, "update_fs_range failed\n");
		goto err2;
	}

	err = l3g4200d_update_odr(gyro, gyro->pdata->poll_interval);
	if (err < 0) {
		dev_err(&client->dev, "update_odr failed\n");
		goto err2;
	}


	err = l3g4200d_input_init(gyro);
	if (err < 0)
		goto err3;


/*    l3g4200d_set_delay(gyro, L3G4200D_DEFAULT_DELAY);*/

	err = sysfs_create_group(&gyro->input->dev.kobj,
						&l3g4200d_attribute_group);

	if (err < 0) {
			dev_err(&gyro->input->dev,
				"%s device register failed\n", L3G4200D_DEV_NAME);
		goto err4;
	}
#if 0
	err = create_sysfs_interfaces(&client->dev);
	if (err < 0) {
		dev_err(&client->dev,
			"%s device register failed\n", L3G4200D_DEV_NAME);
		goto err4;
	}
#endif
	l3g4200d_device_power_off(gyro);

	/* As default, do not report information */
	atomic_set(&gyro->enabled, 0);

	mutex_unlock(&gyro->lock);

	#if DEBUG
	pr_info("%s probed: device created successfully\n", L3G4200D_DEV_NAME);
	#endif
	printk("DBJtech l3g4200d succeed\n");
    return 0;

err4:
	l3g4200d_input_cleanup(gyro);
err3:
	l3g4200d_device_power_off(gyro);
err2:
	if (gyro->pdata->exit)
		gyro->pdata->exit();
err1_1:
	mutex_unlock(&gyro->lock);
	kfree(gyro->pdata);
err1:
	kfree(gyro);
err0:
		pr_err("%s: Driver Initialization failed\n", L3G4200D_DEV_NAME);
		return err;
}

static int l3g4200d_remove(struct i2c_client *client)
{
	struct l3g4200d_data *gyro = i2c_get_clientdata(client);
	#if DEBUG
	pr_info(KERN_INFO "L3G4200D driver removing\n");
	#endif
	l3g4200d_input_cleanup(gyro);
	l3g4200d_device_power_off(gyro);

	sysfs_remove_group(&gyro->input->dev.kobj,
						&l3g4200d_attribute_group);
#if 0
	remove_sysfs_interfaces(&client->dev);
#endif
	kfree(gyro->pdata);
	kfree(gyro);
	return 0;
}

static int l3g4200d_suspend(struct i2c_client *client, pm_message_t mesg)
{
	struct l3g4200d_data *gyro = i2c_get_clientdata(client);
	printk("l3g4200d enter suspend.\n");
	#if DEBUG
	pr_info(KERN_INFO "l3g4200d_suspend\n");
	#endif
	/* TO DO */

	if(atomic_cmpxchg(&gyro->enabled, 1, 0)){
		cancel_delayed_work_sync(&gyro->work);
		l3g4200d_device_power_off(gyro);
	}



	return 0;
}

static int l3g4200d_resume(struct i2c_client *client)
{
	struct l3g4200d_data *gyro = i2c_get_clientdata(client);
	printk("l3g4200d enter resume.\n");
	#if DEBUG
	pr_info(KERN_INFO "l3g4200d_resume\n");
	#endif
	/* TO DO */

	int delay = 0;

	delay = atomic_read(&gyro->delay);

	if (!atomic_cmpxchg(&gyro->enabled, 0, 1)) {

		l3g4200d_device_power_on(gyro);
		schedule_delayed_work(&gyro->work,
				msecs_to_jiffies(delay) + 1);
	}



	return 0;
}


static const struct i2c_device_id l3g4200d_id[] = {
	{ L3G4200D_DEV_NAME , 0 },
	{},
};

MODULE_DEVICE_TABLE(i2c, l3g4200d_id);

#if 0
static struct dev_pm_ops l3g4200d_pm = {
	.suspend = l3g4200d_suspend,
	.resume = l3g4200d_resume,
};
#endif

static struct i2c_driver l3g4200d_driver = {
	.driver = {
			.owner = THIS_MODULE,
			.name = L3G4200D_DEV_NAME,
/*            .pm = &l3g4200d_pm,*/
	},
	.probe = l3g4200d_probe,
	.remove = __devexit_p(l3g4200d_remove),
	.suspend = l3g4200d_suspend,
	.resume  = l3g4200d_resume,
	.id_table = l3g4200d_id,

};

static int __init l3g4200d_init(void)
{
	#if DEBUG
	pr_info("%s: gyroscope sysfs driver init\n", L3G4200D_DEV_NAME);
	#endif
	return i2c_add_driver(&l3g4200d_driver);
}

static void __exit l3g4200d_exit(void)
{
	#if DEBUG
	pr_info("L3G4200D exit\n");
	#endif
	i2c_del_driver(&l3g4200d_driver);
	return;
}

module_init(l3g4200d_init);
module_exit(l3g4200d_exit);

MODULE_DESCRIPTION("l3g4200d digital gyroscope sysfs driver");
MODULE_AUTHOR("STMicroelectronics");
MODULE_LICENSE("GPL");

