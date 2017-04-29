/*
 * I2C driver for Marvell 88PM860x
 *
 * Copyright (C) 2009 Marvell International Ltd.
 * 	Haojian Zhuang <haojian.zhuang@marvell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/mfd/88pm860x.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

static struct i2c_client *pm8607_i2c_client;


#ifdef	CONFIG_PROC_FS
#define PM860_PROC_FILE		"driver/88pm860x"
#define PM860X_POWER_REG_NUM	0xef
static struct proc_dir_entry *pm860x_proc_file;
static int index;

/*
 * (1) cat /proc/driver/88pm860x
 * will print the value of register located in address of index.
 */
static ssize_t pm860x_proc_read(struct file *filp,
	char __user *buffer, size_t count, loff_t *offset)
{
	u8 reg_val;
	ssize_t len;
	const char fmt[] = "register 0x%02x: 0x%02x\n";
	char buf[sizeof(fmt)];

	if (!pm8607_i2c_client)
		return 0;

	if (index == 0xffff) {
		int i;
		pr_info("pm860x:sanremo reg dump\n");
		for (i = 0; i < PM860X_POWER_REG_NUM; i++) {
			reg_val = pm860x_reg_read(pm8607_i2c_client, i);
			pr_info("[0x%02x]=0x%02x\n", i, reg_val);
		}
		return 0;
	}

	if (index < 0 || index > PM860X_POWER_REG_NUM)
		return 0;
	reg_val = pm860x_reg_read(pm8607_i2c_client, index);

	len = sprintf(buf, fmt, index, reg_val);
	return simple_read_from_buffer(buffer, count, offset, buf, len);
}

/*
 * (1) echo -0x0b > /proc/driver/88pm860x
 * will set index to 0x0b such that next proc_read will be done
 * from this new index value.
 *
 * (2) echo 0x0b > /proc/driver/88pm860x
 * will set value of 0x0b for the register in address of index
 *
* */
static ssize_t pm860x_proc_write(struct file *filp,
		const char *buff, size_t len, loff_t *off)
{
	u8 reg_val;
	char messages[256], vol[256];

	if (len > 256)
		len = 256;

	if (copy_from_user(messages, buff, len))
		return -EFAULT;

	if ('-' == messages[0]) {
		/* set the register index */
		memcpy(vol, messages+1, len-1);
		index = (int) simple_strtoul(vol, NULL, 16);
		pr_info("\n88pm860x_proc_write:register # was set = 0x%x\n",
				index);
	} else if ('?' == messages[0]) {
		pr_info("\n write: echo -0x48 > /proc/driver/88pm860x\n");
		pr_info(" read(reg from write): cat /proc/driver/88pm860x\n\n");
	} else{
		/* set the register value */
		reg_val = (int)simple_strtoul(messages, NULL, 16);
		pm860x_reg_write(pm8607_i2c_client, index, (reg_val & 0xFF));
		pr_info("88pm860x_proc_write:register write 0x%x <- %x\n",
				index, (reg_val & 0xFF));
	}
	return len;
}

static const struct file_operations pm860x_proc_ops = {
	.read = pm860x_proc_read,
	.write = pm860x_proc_write,
};

static void create_88pm860x_proc_file(void)
{
	pm860x_proc_file = create_proc_entry(PM860_PROC_FILE, 0644, NULL);
	if (pm860x_proc_file)
		pm860x_proc_file->proc_fops = &pm860x_proc_ops;
	else
		printk(KERN_INFO "proc file create failed!\n");
}

static void remove_88pm860x_proc_file(void)
{
	remove_proc_entry(PM860_PROC_FILE, NULL);
}

#endif

static inline int pm860x_read_device(struct i2c_client *i2c,
				     int reg, int bytes, void *dest)
{
	int ret;

	if (bytes > 1)
		ret = i2c_smbus_read_i2c_block_data(i2c, reg, bytes, dest);
	else {
		ret = i2c_smbus_read_byte_data(i2c, reg);
		if (ret < 0)
			return ret;
		*(unsigned char *)dest = (unsigned char)ret;
	}
	return ret;
}

static inline int pm860x_write_device(struct i2c_client *i2c,
				      int reg, int bytes, void *src)
{
	unsigned char buf[bytes + 1];
	int ret;

	buf[0] = (unsigned char)reg;
	memcpy(&buf[1], src, bytes);

	ret = i2c_master_send(i2c, buf, bytes + 1);
	if (ret < 0)
		return ret;
	return 0;
}

int pm860x_reg_read(struct i2c_client *i2c, int reg)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	unsigned char data = 0;
	int ret;

	mutex_lock(&chip->io_lock);
	ret = pm860x_read_device(i2c, reg, 1, &data);
	mutex_unlock(&chip->io_lock);

	if (ret < 0)
		return ret;
	else
		return (int)data;
}
EXPORT_SYMBOL(pm860x_reg_read);

int pm860x_reg_write(struct i2c_client *i2c, int reg,
		     unsigned char data)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&chip->io_lock);
	ret = pm860x_write_device(i2c, reg, 1, &data);
	mutex_unlock(&chip->io_lock);

	return ret;
}
EXPORT_SYMBOL(pm860x_reg_write);

int pm860x_codec_reg_read(int reg)
{
	struct pm860x_chip *chip = i2c_get_clientdata(pm8607_i2c_client);
	unsigned char data = 0;
	int ret;

	mutex_lock(&chip->io_lock);
	ret = pm860x_read_device(pm8607_i2c_client, reg, 1, &data);
	mutex_unlock(&chip->io_lock);

	if (ret < 0)
		return ret;
	else
		return (int)data;
}
EXPORT_SYMBOL(pm860x_codec_reg_read);

int pm860x_codec_reg_write(int reg, unsigned char data)
{
	struct pm860x_chip *chip = i2c_get_clientdata(pm8607_i2c_client);
	int ret;

	mutex_lock(&chip->io_lock);
	ret = pm860x_write_device(pm8607_i2c_client, reg, 1, &data);
	mutex_unlock(&chip->io_lock);

	return ret;
}
EXPORT_SYMBOL(pm860x_codec_reg_write);

int pm860x_codec_reg_set_bits(int reg, unsigned char mask,
			unsigned char data)
{
	int ret;
	/*we have mutex protection in pm860x_set_bits()*/
	ret = pm860x_set_bits(pm8607_i2c_client, reg, mask, data);
	return ret;
}
EXPORT_SYMBOL(pm860x_codec_reg_set_bits);

int pm860x_bulk_read(struct i2c_client *i2c, int reg,
		     int count, unsigned char *buf)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&chip->io_lock);
	ret = pm860x_read_device(i2c, reg, count, buf);
	mutex_unlock(&chip->io_lock);

	return ret;
}
EXPORT_SYMBOL(pm860x_bulk_read);

int pm860x_bulk_write(struct i2c_client *i2c, int reg,
		      int count, unsigned char *buf)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&chip->io_lock);
	ret = pm860x_write_device(i2c, reg, count, buf);
	mutex_unlock(&chip->io_lock);

	return ret;
}
EXPORT_SYMBOL(pm860x_bulk_write);

int pm860x_set_bits(struct i2c_client *i2c, int reg,
		    unsigned char mask, unsigned char data)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	unsigned char value;
	int ret;

	mutex_lock(&chip->io_lock);
	ret = pm860x_read_device(i2c, reg, 1, &value);
	if (ret < 0)
		goto out;
	value &= ~mask;
	value |= data;
	ret = pm860x_write_device(i2c, reg, 1, &value);
out:
	mutex_unlock(&chip->io_lock);
	return ret;
}
EXPORT_SYMBOL(pm860x_set_bits);

static int read_device(struct i2c_client *i2c, int reg,
		       int bytes, void *dest)
{
	unsigned char msgbuf0[I2C_SMBUS_BLOCK_MAX + 3];
	unsigned char msgbuf1[I2C_SMBUS_BLOCK_MAX + 2];
	struct i2c_adapter *adap = i2c->adapter;
	struct i2c_msg msg[2] = {{i2c->addr, 0, 1, msgbuf0},
				 {i2c->addr, I2C_M_RD, 0, msgbuf1},
				};
	int num = 1, ret = 0;

	if (dest == NULL)
		return -EINVAL;
	msgbuf0[0] = (unsigned char)reg;	/* command */
	msg[1].len = bytes;

	/* if data needs to read back, num should be 2 */
	if (bytes > 0)
		num = 2;
	ret = adap->algo->master_xfer(adap, msg, num);
	memcpy(dest, msgbuf1, bytes);
	return ret;
}

static int write_device(struct i2c_client *i2c, int reg,
			int bytes, void *src)
{
	unsigned char buf[bytes + 1];
	struct i2c_adapter *adap = i2c->adapter;
	struct i2c_msg msg;
	int ret;

	buf[0] = (unsigned char)reg;
	memcpy(&buf[1], src, bytes);
	msg.addr = i2c->addr;
	msg.flags = 0;
	msg.len = bytes + 1;
	msg.buf = buf;

	ret = adap->algo->master_xfer(adap, &msg, 1);
	if (ret < 0)
		return ret;
	return 0;
}

int pm860x_page_reg_read(struct i2c_client *i2c, int reg)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	unsigned char zero = 0;
	unsigned char data;
	int ret;

	mutex_lock(&chip->io_lock);
	i2c_lock_adapter(i2c->adapter);
	if (i2c->adapter->hardware_lock)
		i2c->adapter->hardware_lock();
	read_device(i2c, 0xFA, 0, &zero);
	read_device(i2c, 0xFB, 0, &zero);
	read_device(i2c, 0xFF, 0, &zero);
	ret = read_device(i2c, reg, 1, &data);
	if (ret >= 0)
		ret = (int)data;
	read_device(i2c, 0xFC, 0, &zero);
	read_device(i2c, 0xFE, 0, &zero);
	if (i2c->adapter->hardware_unlock)
		i2c->adapter->hardware_unlock();
	i2c_unlock_adapter(i2c->adapter);
	mutex_unlock(&chip->io_lock);
	return ret;
}
EXPORT_SYMBOL(pm860x_page_reg_read);

int pm860x_page_reg_write(struct i2c_client *i2c, int reg,
			  unsigned char data)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	unsigned char zero;
	int ret;

	mutex_lock(&chip->io_lock);
	i2c_lock_adapter(i2c->adapter);
	if (i2c->adapter->hardware_lock)
		i2c->adapter->hardware_lock();
	read_device(i2c, 0xFA, 0, &zero);
	read_device(i2c, 0xFB, 0, &zero);
	read_device(i2c, 0xFF, 0, &zero);
	ret = write_device(i2c, reg, 1, &data);
	read_device(i2c, 0xFC, 0, &zero);
	read_device(i2c, 0xFE, 0, &zero);
	if (i2c->adapter->hardware_unlock)
		i2c->adapter->hardware_unlock();
	i2c_unlock_adapter(i2c->adapter);
	mutex_unlock(&chip->io_lock);
	return ret;
}
EXPORT_SYMBOL(pm860x_page_reg_write);

int pm860x_page_bulk_read(struct i2c_client *i2c, int reg,
			  int count, unsigned char *buf)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	unsigned char zero = 0;
	int ret;

	mutex_lock(&chip->io_lock);
	i2c_lock_adapter(i2c->adapter);
	if (i2c->adapter->hardware_lock)
		i2c->adapter->hardware_lock();
	read_device(i2c, 0xFA, 0, &zero);
	read_device(i2c, 0xFB, 0, &zero);
	read_device(i2c, 0xFF, 0, &zero);
	ret = read_device(i2c, reg, count, buf);
	read_device(i2c, 0xFC, 0, &zero);
	read_device(i2c, 0xFE, 0, &zero);
	if (i2c->adapter->hardware_unlock)
		i2c->adapter->hardware_unlock();
	i2c_unlock_adapter(i2c->adapter);
	mutex_unlock(&chip->io_lock);
	return ret;
}
EXPORT_SYMBOL(pm860x_page_bulk_read);

int pm860x_page_bulk_write(struct i2c_client *i2c, int reg,
			   int count, unsigned char *buf)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	unsigned char zero = 0;
	int ret;

	mutex_lock(&chip->io_lock);
	i2c_lock_adapter(i2c->adapter);
	if (i2c->adapter->hardware_lock)
		i2c->adapter->hardware_lock();
	read_device(i2c, 0xFA, 0, &zero);
	read_device(i2c, 0xFB, 0, &zero);
	read_device(i2c, 0xFF, 0, &zero);
	ret = write_device(i2c, reg, count, buf);
	read_device(i2c, 0xFC, 0, &zero);
	read_device(i2c, 0xFE, 0, &zero);
	if (i2c->adapter->hardware_unlock)
		i2c->adapter->hardware_unlock();
	i2c_unlock_adapter(i2c->adapter);
	mutex_unlock(&chip->io_lock);
	return ret;
}
EXPORT_SYMBOL(pm860x_page_bulk_write);

int pm860x_page_set_bits(struct i2c_client *i2c, int reg,
			 unsigned char mask, unsigned char data)
{
	struct pm860x_chip *chip = i2c_get_clientdata(i2c);
	unsigned char zero;
	unsigned char value;
	int ret;

	mutex_lock(&chip->io_lock);
	i2c_lock_adapter(i2c->adapter);
	if (i2c->adapter->hardware_lock)
		i2c->adapter->hardware_lock();
	read_device(i2c, 0xFA, 0, &zero);
	read_device(i2c, 0xFB, 0, &zero);
	read_device(i2c, 0xFF, 0, &zero);
	ret = read_device(i2c, reg, 1, &value);
	if (ret < 0)
		goto out;
	value &= ~mask;
	value |= data;
	ret = write_device(i2c, reg, 1, &value);
out:
	read_device(i2c, 0xFC, 0, &zero);
	read_device(i2c, 0xFE, 0, &zero);
	if (i2c->adapter->hardware_unlock)
		i2c->adapter->hardware_unlock();
	i2c_unlock_adapter(i2c->adapter);
	mutex_unlock(&chip->io_lock);
	return ret;
}
EXPORT_SYMBOL(pm860x_page_set_bits);

static const struct i2c_device_id pm860x_id_table[] = {
	{ "88PM860x", 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, pm860x_id_table);

static int verify_addr(struct i2c_client *i2c)
{
	unsigned short addr_8607[] = {0x30, 0x34};
	unsigned short addr_8606[] = {0x10, 0x11};
	int size, i;

	if (i2c == NULL)
		return 0;
	size = ARRAY_SIZE(addr_8606);
	for (i = 0; i < size; i++) {
		if (i2c->addr == *(addr_8606 + i))
			return CHIP_PM8606;
	}
	size = ARRAY_SIZE(addr_8607);
	for (i = 0; i < size; i++) {
		if (i2c->addr == *(addr_8607 + i))
			return CHIP_PM8607;
	}
	return 0;
}

static int __devinit pm860x_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
	struct pm860x_platform_data *pdata = client->dev.platform_data;
	struct pm860x_chip *chip;

	if (!pdata) {
		pr_info("No platform data in %s!\n", __func__);
		return -EINVAL;
	}

	chip = kzalloc(sizeof(struct pm860x_chip), GFP_KERNEL);
	if (chip == NULL)
		return -ENOMEM;

	chip->id = verify_addr(client);
	chip->client = client;
	pm8607_i2c_client = client;
	i2c_set_clientdata(client, chip);
	chip->dev = &client->dev;
	mutex_init(&chip->io_lock);
	dev_set_drvdata(chip->dev, chip);

	/*
	 * Both client and companion client shares same platform driver.
	 * Driver distinguishes them by pdata->companion_addr.
	 * pdata->companion_addr is only assigned if companion chip exists.
	 * At the same time, the companion_addr shouldn't equal to client
	 * address.
	 */
	if (pdata->companion_addr && (pdata->companion_addr != client->addr)) {
		chip->companion_addr = pdata->companion_addr;
		chip->companion = i2c_new_dummy(chip->client->adapter,
						chip->companion_addr);
		i2c_set_clientdata(chip->companion, chip);
	}

	if (pdata->fixup)
		pdata->fixup(chip, pdata);
	pm860x_device_init(chip, pdata);

#ifdef	CONFIG_PROC_FS
	create_88pm860x_proc_file();
#endif


	return 0;
}

static int __devexit pm860x_remove(struct i2c_client *client)
{
	struct pm860x_chip *chip = i2c_get_clientdata(client);

#ifdef	CONFIG_PROC_FS
	remove_88pm860x_proc_file();
#endif
	pm860x_device_exit(chip);
	i2c_unregister_device(chip->companion);
	kfree(chip);
	return 0;
}

static struct i2c_driver pm860x_driver = {
	.driver	= {
		.name	= "88PM860x",
		.owner	= THIS_MODULE,
	},
	.probe		= pm860x_probe,
	.remove		= __devexit_p(pm860x_remove),
	.id_table	= pm860x_id_table,
};

static int __init pm860x_i2c_init(void)
{
	int ret;
	ret = i2c_add_driver(&pm860x_driver);
	if (ret != 0)
		pr_err("Failed to register 88PM860x I2C driver: %d\n", ret);
	return ret;
}
subsys_initcall(pm860x_i2c_init);

static void __exit pm860x_i2c_exit(void)
{
	i2c_del_driver(&pm860x_driver);
}
module_exit(pm860x_i2c_exit);

MODULE_DESCRIPTION("I2C Driver for Marvell 88PM860x");
MODULE_AUTHOR("Haojian Zhuang <haojian.zhuang@marvell.com>");
MODULE_LICENSE("GPL");
