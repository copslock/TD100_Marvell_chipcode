/*
 * * Copyright (C) 2009, Notioni Corporation chenjian@notioni.com).
 * *
 * * Author: jeremy.chen
 * *
 * * This software program is licensed subject to the GNU General Public License
 * * (GPL).Version 2,June 1991, available at http://www.fsf.org/copyleft/gpl.html
 * */
#define DEBUG 0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/i2c.h>
#include <linux/suspend.h>
#include <linux/input.h>
#include <asm/uaccess.h>
#include <mach/gpio.h>
#include <plat/gpio.h>
#include <linux/earlysuspend.h>
#include <linux/i2c/ft5306_touch.h>
#include <plat/mfp.h>
#include <mach/dbj.h>


struct ft5306_touch {
	struct input_dev *idev;
	struct i2c_client *i2c;
	struct work_struct work;
	struct ft5306_touch_platform_data *data;
	int pen_status;
	int irq;
};
static struct ft5306_touch *touch;

#define FT5306_LEN 0x3E
static u8 ft5306_buf[FT5306_LEN];
static u8 ft5306_mode_cmd_sleep[2] = { 0xA5, 0x03 };
static u8 ft5306_cmd[2] = { 0x0, 0x0 };



#define FT5306_PEN_UP	0
#define FT5306_PEN_DOWN	1

static void virtual_keys_init(void);

int ft5306_touch_read_reg(u8 reg, u8 * pval)
{
	int ret;
	int status;

	if (touch->i2c == NULL)
		return -1;
	ret = i2c_smbus_read_byte_data(touch->i2c, reg);
	if (ret >= 0) {
		*pval = ret;
		status = 0;
	} else {
		status = -EIO;
	}

	return status;
}

int ft5306_touch_write_reg(u8 reg, u8 val)
{
	int ret;
	int status;

	if (touch->i2c == NULL)
		return -1;
	ret = i2c_smbus_write_byte_data(touch->i2c, reg, val);
	if (ret == 0)
		status = 0;
	else
		status = -EIO;

	return status;
}

#ifdef CONFIG_TD100_JLJ
static int ft5306_touch_read(char *buf, int count)
{
  	int ret;

	if (touch->i2c == NULL)
		return -1;

	struct i2c_msg msgs[] = {
		{
			.addr	= touch->i2c->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= buf,
		},
		{
			.addr	= touch->i2c->addr,
			.flags	= I2C_M_RD,
			.len	= count,
			.buf	= buf,
		},
	};

    //msleep(1);
	ret = i2c_transfer(touch->i2c->adapter, msgs, 2);
	if (ret < 0)
		pr_err("msg %s i2c read error: %d\n", __func__, ret);
	
	return ret;
}
#else
static int ft5306_touch_read(char *buf, int count)
{
	int ret;

	ret = i2c_master_recv(touch->i2c, (char *) buf, count);

	return ret;
}
#endif

static int ft5306_touch_write(char *buf, int count)
{
	int ret;

	ret = i2c_master_send(touch->i2c, buf, count);

	return ret;
}

static int ft5306_touch_recieve_data(void)
{
	return ft5306_touch_read(ft5306_buf, FT5306_LEN);
}

static void ft5306_touch_work(struct work_struct *work)
{
	u16 tem_x1 = 0;
	u16 tem_y1 = 0;
	u16 tem_x2 = 0;
	u16 tem_y2 = 0;
	u8 tmp;
	u8 ret;

	ret = ft5306_touch_read(ft5306_buf, 31);

	tem_x1 = ((u16) (ft5306_buf[3] & 0xF) << 8) | (u16) ft5306_buf[4];
	tem_y1 = ((u16) (ft5306_buf[5] & 0xF) << 8) | (u16) ft5306_buf[6];
	tem_x2 = ((u16) (ft5306_buf[9] & 0xF) << 8) | (u16) ft5306_buf[10];
	tem_y2 =
	    ((u16) (ft5306_buf[11] & 0xF) << 8) | (u16) ft5306_buf[12];

#if DEBUG
	printk(KERN_DEBUG "cj:ft5306_touch.c----x1:%d,y1:%d;x2:%d,y2:%d.\n", tem_x1,
	       tem_y1, tem_x2, tem_y2);
#endif
	tmp = ft5306_buf[2] & 0x07;
	if (tmp == 1) {
		/* One finger */
#if DEBUG
		printk(KERN_DEBUG "cj:ft5306_touch.c----One finger.\n");
#endif
		touch->pen_status = FT5306_PEN_DOWN;

		input_report_abs(touch->idev, ABS_MT_TRACKING_ID, 0);
		input_report_abs(touch->idev, ABS_PRESSURE, 255);
		input_report_abs(touch->idev, ABS_X, tem_x1);
		input_report_abs(touch->idev, ABS_Y, tem_y1);
		input_report_abs(touch->idev, ABS_MT_TOUCH_MAJOR, 16);
		input_report_abs(touch->idev, ABS_MT_POSITION_X, tem_x1);
		input_report_abs(touch->idev, ABS_MT_POSITION_Y, tem_y1);
		input_report_key(touch->idev, BTN_TOUCH, 1);
		input_mt_sync(touch->idev);
		input_sync(touch->idev);
	} else if (tmp == 0x2) {
		/* Two fingers */
#if DEBUG
		printk(KERN_DEBUG "ft5306_touch.c----Two finger.\n");
#endif
		touch->pen_status = FT5306_PEN_DOWN;

		input_report_abs(touch->idev, ABS_MT_TRACKING_ID, 0);
		input_report_abs(touch->idev, ABS_PRESSURE, 255);
		input_report_abs(touch->idev, ABS_X, tem_x1);
		input_report_abs(touch->idev, ABS_Y, tem_y1);
		input_report_abs(touch->idev, ABS_MT_TOUCH_MAJOR, 16);
		input_report_abs(touch->idev, ABS_MT_POSITION_X, tem_x1);
		input_report_abs(touch->idev, ABS_MT_POSITION_Y, tem_y1);
		input_report_key(touch->idev, BTN_TOUCH, 1);
		input_mt_sync(touch->idev);

		input_report_abs(touch->idev, ABS_MT_TRACKING_ID, 1);
		input_report_abs(touch->idev, ABS_PRESSURE, 255);
		input_report_abs(touch->idev, ABS_X, tem_x2);
		input_report_abs(touch->idev, ABS_Y, tem_y2);
		input_report_abs(touch->idev, ABS_MT_TOUCH_MAJOR, 16);
		input_report_abs(touch->idev, ABS_MT_POSITION_X, tem_x2);
		input_report_abs(touch->idev, ABS_MT_POSITION_Y, tem_y2);
		input_report_key(touch->idev, BTN_2, 1);
		input_mt_sync(touch->idev);

		input_sync(touch->idev);
	} else if (tmp == 0) {
		/* No finger */
#if DEBUG
		printk(KERN_DEBUG "cj:ft5306_touch.c----No finger.\n");
#endif
		touch->pen_status = FT5306_PEN_UP;

		input_report_abs(touch->idev, ABS_PRESSURE, 0);
		input_report_key(touch->idev, BTN_TOUCH, 0);
		input_report_key(touch->idev, BTN_2, 0);
		input_report_abs(touch->idev, ABS_MT_TOUCH_MAJOR, 0);
		input_sync(touch->idev);
	}
}

static irqreturn_t ft5306_touch_irq_handler(int irq, void *dev_id)
{
/*    printk( "ft5306_touch.c----ft5306_touch_irq_handler.\n");*/

	schedule_work(&touch->work);

	return IRQ_HANDLED;
}

#ifdef	CONFIG_PM
static int
ft5306_touch_suspend(struct i2c_client *client, pm_message_t state)
{
	return 0;
}

static int ft5306_touch_resume(struct i2c_client *client)
{
	return 0;
}
#else
#define	ft5306_touch_suspend		NULL
#define	ft5306_touch_resume		NULL
#endif

#ifdef CONFIG_PROC_FS
#define	FT5306_TOUCH_PROC_FILE	"driver/ft5306_touch"
static struct proc_dir_entry *ft5306_touch_proc_file;
static int index;

static ssize_t
ft5306_touch_proc_read(struct file *filp,
		       char *buffer, size_t length, loff_t * offset)
{
	u8 reg_val;

	if ((index < 0) || (index > FT5306_LEN))
		return 0;

	ft5306_touch_read_reg(index, (u8 *)&reg_val);
	printk(KERN_INFO "register 0x%x: 0x%x\n", index, reg_val);
	return 0;
}

static ssize_t
ft5306_touch_proc_write(struct file *filp,
			const char *buff, size_t len, loff_t * off)
{
	int ret;
	char messages[256], vol[256] = { 0 };
	u32 reg = 0, val = 0;
	int i;


	if (len > 256)
		len = 256;

	if (copy_from_user(messages, buff, len))
		return -EFAULT;

	if ('w' == messages[0]) {
		memcpy(vol, messages + 2, 4);
		reg = (int) simple_strtoul(vol, NULL, 16);
		memcpy(vol, messages + 7, 4);
		val = (int) simple_strtoul(vol, NULL, 16);
		ft5306_cmd[0] = reg;
		ft5306_cmd[1] = val;
		ret = ft5306_touch_write(ft5306_cmd, 2);
		printk(KERN_INFO "write! reg:0x%x, val:0x%x\n", reg, val);

	} else if ('r' == messages[0]) {
		memcpy(vol, messages + 2, 4);
		reg = (int) simple_strtoul(vol, NULL, 16);
		ret = ft5306_touch_read_reg(reg, (u8 *)&val);
		printk(KERN_INFO "Read! reg:0x%x, val:0x%x\n", reg, val);

	} else if ('d' == messages[0]) {
		for (i = 0x00; i <= 0x3E; i++) {
			reg = i;
			ft5306_touch_read_reg(reg, (u8 *)&val);
			msleep(2);
			printk(KERN_INFO "Display! reg:0x%x, val:0x%x\n",
			       reg, val);
		}
	}
	return len;
}

static struct file_operations ft5306_touch_proc_ops = {
	.read = ft5306_touch_proc_read,
	.write = ft5306_touch_proc_write,
};

static void create_ft5306_touch_proc_file(void)
{
	ft5306_touch_proc_file =
	    create_proc_entry(FT5306_TOUCH_PROC_FILE, 0644, NULL);
	if (ft5306_touch_proc_file) {
		ft5306_touch_proc_file->proc_fops = &ft5306_touch_proc_ops;
	} else
		printk(KERN_INFO "proc file create failed!\n");
}

static void remove_ft5306_touch_proc_file(void)
{
	extern struct proc_dir_entry proc_root;
	remove_proc_entry(FT5306_TOUCH_PROC_FILE, &proc_root);
}
#endif

static int ft5306_touch_open(struct input_dev *idev)
{
	touch->data->power(1);
	return 0;
}

static void ft5306_touch_close(struct input_dev *idev)
{
	touch->data->power(0);
	return;
}

static void ft5306_touch_sleep_early_suspend(struct early_suspend *h)
{
	int ret, i = 0;
#if 1
      sleep_retry:
	ret = ft5306_touch_write(ft5306_mode_cmd_sleep, 2);
	if (ret < 0) {
		if (i < 50) {
			msleep(5);
			i++;
			printk(KERN_WARNING
			       "ft5306_touch can't enter sleep,retry %d\n",
			       i);
			goto sleep_retry;
		}
		printk(KERN_WARNING "ft5306_touch can't enter sleep\n");
		return;
	} else {
		printk(KERN_INFO "ft5306_touch enter sleep mode.\n");
	}
	touch->data->power(0);

#endif
}

#ifndef CONFIG_TD100_JLJ
static void ft5306_touch_normal_late_resume(struct early_suspend *h)
{
	if (gpio_request(MFP_PIN_GPIO11, "ft5306_wake")) {
		pr_err("Failed to request GPIO for ft5306_wake pin!\n");
		goto out;
	}
	gpio_direction_output(MFP_PIN_GPIO11, 0);
	mdelay(10);
	gpio_direction_output(MFP_PIN_GPIO11, 1);
	printk(KERN_INFO "ft5306_touch resume successful.\n");
	gpio_free(MFP_PIN_GPIO11);
out:
	return;
}
#else
static void ft5306_touch_normal_late_resume(struct early_suspend *h)
{
	touch->data->power(1);
	if (touch->data->reset)
		touch->data->reset();
}
#endif

static struct early_suspend ft5306_touch_early_suspend_desc = {
	.level = EARLY_SUSPEND_LEVEL_STOP_DRAWING,
	.suspend = ft5306_touch_sleep_early_suspend,
	.resume = ft5306_touch_normal_late_resume,
};


static int __devinit
ft5306_touch_probe(struct i2c_client *client,
		   const struct i2c_device_id *id)
{
	int ret;
	u8 reg_val;

	printk("ft5306_touch.c----ft5306_touch_probe.\n");

	touch = kzalloc(sizeof(struct ft5306_touch), GFP_KERNEL);
	if (touch == NULL)
		return -ENOMEM;

	touch->data = client->dev.platform_data;
	if (touch->data == NULL) {
		dev_dbg(&client->dev, "no platform data\n");
		return -EINVAL;
	}
	touch->i2c = client;
	touch->irq = client->irq;
	touch->pen_status = FT5306_PEN_UP;
	touch->data->power(1);

	ret = ft5306_touch_read_reg(0x00, (u8 *)&reg_val);
	if (ret < 0) {
		printk(KERN_WARNING "ft5306 detect fail!\n");
		touch->i2c = NULL;
		return -ENXIO;
	} else {
		printk(KERN_INFO "ft5306 reset.\n");
	}

	if (touch->data->reset)
		touch->data->reset();

	/* register input device */
	touch->idev = input_allocate_device();
	if (touch->idev == NULL) {
		printk(KERN_ERR "%s: failed to allocate input dev\n",
		       __FUNCTION__);
		ret = -ENOMEM;
		goto out;
	}

	touch->idev->name = "ft5306-ts";
	touch->idev->phys = "ft5306-ts/input0";
	touch->idev->open = ft5306_touch_open;
	touch->idev->close = ft5306_touch_close;

	__set_bit(EV_ABS, touch->idev->evbit);
	__set_bit(ABS_X, touch->idev->absbit);
	__set_bit(ABS_Y, touch->idev->absbit);
	__set_bit(ABS_MT_POSITION_X, touch->idev->absbit);
	__set_bit(ABS_MT_POSITION_Y, touch->idev->absbit);
	__set_bit(ABS_PRESSURE, touch->idev->absbit);

	__set_bit(EV_SYN, touch->idev->evbit);
	__set_bit(EV_KEY, touch->idev->evbit);
	__set_bit(BTN_TOUCH, touch->idev->keybit);
	__set_bit(BTN_2, touch->idev->keybit);

	input_set_abs_params(touch->idev, ABS_MT_TOUCH_MAJOR, 0, 16, 0, 0);
	input_set_abs_params(touch->idev, ABS_X, 0, 480, 0, 0);
	input_set_abs_params(touch->idev, ABS_Y, 0, 800, 0, 0);
	input_set_abs_params(touch->idev, ABS_PRESSURE, 0, 255, 0, 0);

	/*   muti touch */
	input_set_abs_params(touch->idev, ABS_MT_POSITION_X, 0, 480, 0, 0);
	input_set_abs_params(touch->idev, ABS_MT_POSITION_Y, 0, 800, 0, 0);

	ret = input_register_device(touch->idev);
	if (ret) {
		printk(KERN_ERR
		       "%s: unabled to register input device, ret = %d\n",
		       __FUNCTION__, ret);
		goto out_rg;
	}

	ret = request_irq(touch->irq, ft5306_touch_irq_handler,
			  IRQF_DISABLED | IRQF_TRIGGER_FALLING,
			  "ft5306 touch", touch);
	if (ret < 0)
		goto out_irg;

	if (ret) {
		printk(KERN_WARNING
		       "Request IRQ for Bigstream touch failed, return:%d\n",
		       ret);
		goto out_rg;
	}
	INIT_WORK(&touch->work, ft5306_touch_work);
	register_early_suspend(&ft5306_touch_early_suspend_desc);

#ifdef	CONFIG_PROC_FS
	create_ft5306_touch_proc_file();
#endif

	virtual_keys_init();
/*    touch->data->power(0);*/

	return 0;

      out_irg:
	free_irq(touch->irq, touch);
      out_rg:
	input_free_device(touch->idev);
      out:
	kfree(touch);
	return ret;

}

static int ft5306_touch_remove(struct i2c_client *client)
{
#ifdef CONFIG_PROC_FS
	remove_ft5306_touch_proc_file();
#endif
	input_unregister_device(touch->idev);
	return 0;
}

static const struct i2c_device_id ft5306_touch_id[] = {
	{"ft5306_touch", 0},
	{}
};

static struct i2c_driver ft5306_touch_driver = {
	.driver = {
		   .name = "ft5306_touch",
		   },
	.id_table = ft5306_touch_id,
	.probe = ft5306_touch_probe,
	.remove = ft5306_touch_remove,
	.suspend = ft5306_touch_suspend,
	.resume = ft5306_touch_resume,
};

static int __init ft5306_touch_init(void)
{
	return i2c_add_driver(&ft5306_touch_driver);
}

static void __exit ft5306_touch_exit(void)
{
	unregister_early_suspend(&ft5306_touch_early_suspend_desc);
	i2c_del_driver(&ft5306_touch_driver);
}

#ifdef CONFIG_TD100_JLJ
static ssize_t
virtual_keys_show(struct kobject *kobj,
		  struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, __stringify(EV_KEY) ":" __stringify(KEY_MENU)
		       ":60:900:100:50" ":" __stringify(EV_KEY) ":"
		       __stringify(KEY_HOME) ":180:900:100:50" ":"
		       __stringify(EV_KEY) ":" __stringify(KEY_BACK)
		       ":300:900:100:50" ":" __stringify(EV_KEY) ":"
		       __stringify(KEY_SEARCH) ":420:900:100:50" "\n");
}
#else
static ssize_t
virtual_keys_show(struct kobject *kobj,
		  struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, __stringify(EV_KEY) ":" __stringify(KEY_MENU)
		       ":60:830:100:50" ":" __stringify(EV_KEY) ":"
		       __stringify(KEY_HOME) ":180:830:100:50" ":"
		       __stringify(EV_KEY) ":" __stringify(KEY_BACK)
		       ":300:830:100:50" ":" __stringify(EV_KEY) ":"
		       __stringify(KEY_SEARCH) ":420:830:100:50" "\n");
}
#endif

static struct kobj_attribute virtual_keys_attr = {

	.attr = {

		 .name = "virtualkeys.ft5306-ts",

		 .mode = S_IRUGO,

		 },

	.show = &virtual_keys_show,
};

static struct attribute *properties_attrs[] = {

	&virtual_keys_attr.attr,

	NULL
};

static struct attribute_group properties_attr_group = {

	.attrs = properties_attrs,

};

void virtual_keys_init(void)
{
	int ret;
	struct kobject *properties_kobj;

	//printk("cj: virtual_keys_init.\n");
	properties_kobj = kobject_create_and_add("board_properties", NULL);

	if (properties_kobj)
		ret =
		    sysfs_create_group(properties_kobj,
				       &properties_attr_group);

	if (!properties_kobj || ret)
		pr_err("failed to create board_properties\n");
}

module_init(ft5306_touch_init);
module_exit(ft5306_touch_exit);

MODULE_DESCRIPTION("ft5306 touch Driver");
MODULE_LICENSE("GPL");
