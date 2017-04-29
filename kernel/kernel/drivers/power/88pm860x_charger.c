/*
 * Battery driver for Marvell 88PM860x PMIC
 *
 * Copyright (c) 2009-2010 Marvell International Ltd.
 * Author:	Jett Zhou <jtzhou@marvell.com>
 *		Haojian Zhuang <haojian.zhuang@marvell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#define DEBUG	1
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/power_supply.h>
#include <linux/mfd/88pm860x.h>
#include <asm/div64.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

/* bit definitions of Status Query Interface 2 */
#define STATUS2_CHG		(1 << 2)

/* bit definitions of Reset Out Register */
#define RESET_SW_PD		(1 << 7)

/* bit definitions of PreReg 1 */
#define PREREG1_1500MA		(0x0E)
#define PREREG1_450MA		(0x04)
#define PREREG1_540MA		(0x05)
#define PREREG1_630MA		(0x06)
#define PREREG1_720MA		(0x07)
#define PREREG1_VSYS_4_5V	(3 << 4)
#define PREREG1_VSYS_4_3V	(4 << 4)

/* bit definitions of Charger Control 1 Register */
#define CC1_MODE_OFF		(0)
#define CC1_MODE_PRECHARGE	(1)
#define CC1_MODE_FASTCHARGE	(2)
#define CC1_MODE_PULSECHARGE	(3)
#define CC1_ITERM_20MA		(0 << 2)
#define CC1_ITERM_60MA		(2 << 2)
#define CC1_VFCHG_4_2V		(9 << 4)
#define CC1_VFCHG_4_1V		(7 << 4)

/* bit definitions of Charger Control 2 Register */
#define CC2_ICHG_500MA		(9)
#define CC2_ICHG_1000MA		(0x13)

/* bit definitions of Charger Control 3 Register */
#define CC3_180MIN_TIMEOUT	(0x6 << 4)
#define CC3_270MIN_TIMEOUT	(0x7 << 4)
#define CC3_360MIN_TIMEOUT	(0xA << 4)
#define CC3_DISABLE_TIMEOUT	(0xF << 4)

/* bit definitions of Charger Control 4 Register */
#define CC4_IPRE_40MA		(7)
#define CC4_VPCHG_3_2V		(3 << 4)
#define CC4_IFCHG_MON_EN	(1 << 6)
#define CC4_BTEMP_MON_EN	(1 << 7)

/* bit definitions of Charger Control 6 Register */
#define CC6_BAT_OV_EN		(1 << 2)
#define CC6_BAT_UV_EN		(1 << 3)
#define CC6_UV_VBAT_SET		(0x3 << 6)/*2.8v*/

/* bit definitions of Charger Control 7 Register */
#define CC7_BAT_REM_EN		(1 << 3)
#define CC7_IFSM_EN		(1 << 7)

/* bit definitions of Measurement Enable 1 Register */
#define MEAS1_VBAT		(1 << 0)

/* bit definitions of Measurement Enable 3 Register */
#define MEAS3_IBAT_EN		(1 << 0)
#define MEAS3_CC_EN		(1 << 2)

#define FSM_INIT		0
#define FSM_DISCHARGE		1
#define FSM_PRECHARGE		2
#define FSM_FASTCHARGE		3

#define PRECHARGE_THRESHOLD	3100
#define POWEROFF_THRESHOLD	3400
#define CHARGE_THRESHOLD	4000
#define DISCHARGE_THRESHOLD	4180

/* Temperature is reverse from GPADC1 voltage*/
#define GPADC1_DEGREE_MIN	1462
#define GPADC1_DEGREE_MAX	15
#define DEGREE_MIN		-25
#define DEGREE_MAX		100

/*over-temperature on PM8606 setting*/
#define OVER_TEMP_FLAG		1<<6
#define OVTEMP_AUTORECOVER	1<<3
/*over-voltage protect on vchg setting mv*/
/*#define VCHG_NORMAL_LOW		4200*/
#define VCHG_NORMAL_LOW		4000
#define VCHG_NORMAL_CHECK	5800
#define VCHG_NORMAL_HIGH	6000
#define VCHG_OVP_LOW		5500


struct pm860x_charger_info {
	struct pm860x_chip	*chip;
	struct i2c_client	*i2c;
	struct i2c_client	*i2c_8606;
	struct device		*dev;

	struct power_supply	usb;
	struct power_supply	ac;
	struct mutex		lock;
	struct work_struct	vbus_work;
	int			irq_nums;
	int			irq[7];
	unsigned		state : 3;	/* fsm state */
	unsigned		charge_type : 2;
	unsigned		online : 1; /* PC usb*/
	unsigned		present : 1;	/* battery present */
	unsigned 		allowed : 1;
	unsigned		vbus_output : 1; /* usb host */
	unsigned		bc_short;	/*1 disable 0 enable */
};

static char *pm860x_supplied_to[] = {
	"battery",
};

static struct pm860x_charger_info *ginfo;

static int stop_charge(struct pm860x_charger_info *info, int vbatt);
static int set_charging_fsm(struct pm860x_charger_info *info);
static void set_vbatt_threshold(struct pm860x_charger_info *info,
				int min, int max);
static void set_vchg_threshold(struct pm860x_charger_info *info,
				int min, int max);
static int measure_vchg(struct pm860x_charger_info *info, int *data);
static int measure_vsys(struct pm860x_charger_info *info, int *data);

void pm860x_set_charger_type(enum enum_charger_type type)
{
	struct power_supply *psy;
	union power_supply_propval data;
	struct pm860x_charger_info *info = ginfo;
	int ret = -EINVAL;
	int vbatt, type_changed = 0;

	psy = power_supply_get_by_name(pm860x_supplied_to[0]);
	if (!psy)
		goto out;
	ret = psy->get_property(psy, POWER_SUPPLY_PROP_VOLTAGE_NOW, &data);
	if (ret)
		goto out;
	vbatt = data.intval / 1000;

	dev_dbg(info->dev, "charger type : %s detect...\n",
		(type == USB_CHARGER) ? "usb" : "ac");

	mutex_lock(&info->lock);
	info->online = 1;
	if (info->charge_type != type) {
		switch (type) {
		case USB_CHARGER:
			info->charge_type = USB_CHARGER;
			break;
		case AC_STANDARD_CHARGER:
			info->charge_type = AC_STANDARD_CHARGER;
			break;
		case AC_OTHER_CHARGER:
			info->charge_type = AC_OTHER_CHARGER;
			break;
		default:
			break;
		}
		type_changed = 1;
	}
	if (info->state == FSM_FASTCHARGE && type_changed) {
		info->allowed = 0;
		mutex_unlock(&info->lock);
		set_charging_fsm(info);
		mutex_lock(&info->lock);
		info->allowed = 1;
		mutex_unlock(&info->lock);
		set_charging_fsm(info);
	} else {
		mutex_unlock(&info->lock);
	}
	if (info->charge_type == USB_CHARGER)
		power_supply_changed(&info->usb);
	else
		power_supply_changed(&info->ac);
out:
	return;
}

static void pm860x_vbus_work(struct work_struct *work)
{
	struct pm860x_charger_info *info = container_of(work,
		struct pm860x_charger_info, vbus_work);
	int ret;

	mutex_lock(&info->lock);
	if (info->vbus_output) {
		info->online = 0;
		/* disable VCHG interrupt */
		disable_irq(info->irq[6]);
	} else {
		ret = pm860x_reg_read(info->i2c, PM8607_STATUS_2);
		if (ret < 0) {
			mutex_unlock(&info->lock);
			goto out;
		}
		if (ret & STATUS2_CHG)
			info->online = 1;
		else
			info->online = 0;
		enable_irq(info->irq[6]);
	}
	mutex_unlock(&info->lock);
	dev_dbg(info->dev, "VBUS output:%d, Charger:%s\n", info->vbus_output,
		(info->online) ? "online" : "N/A");
out:
	set_charging_fsm(info);
}

void pm860x_set_vbus_output(int enable)
{
	struct pm860x_charger_info *info = ginfo;

	mutex_lock(&info->lock);
	if (enable)
		enable = 1;
	/* avoid to enable or disable twice */
	if (info->vbus_output == enable) {
		mutex_unlock(&info->lock);
		return;
	}
	info->vbus_output = enable;
	mutex_unlock(&info->lock);
	queue_work(info->chip->monitor_wqueue, &info->vbus_work);
}
EXPORT_SYMBOL(pm860x_set_vbus_output);

irqreturn_t pm860x_charger_handler(int irq, void *data)
{
	struct pm860x_charger_info *info = ginfo;
	int ret;

	mutex_lock(&info->lock);
	ret = pm860x_reg_read(info->i2c, PM8607_STATUS_2);
	if (ret < 0) {
		mutex_unlock(&info->lock);
		goto out;
	}
	if (ret & STATUS2_CHG) {
		info->online = 1;
		info->allowed = 1;
	} else {
		info->online = 0;
		info->allowed = 0;
	}
	mutex_unlock(&info->lock);
	dev_dbg(info->dev, "%s, Charger:%s, Allowed:%d\n", __func__,
		(info->online) ? "online" : "N/A", info->allowed);

	set_charging_fsm(info);

	if (info->charge_type == USB_CHARGER)
		power_supply_changed(&info->usb);
	else
		power_supply_changed(&info->ac);
out:
	return IRQ_HANDLED;
}

static irqreturn_t pm860x_temp_handler(int irq, void *data)
{
	struct power_supply *psy;
	struct pm860x_charger_info *info = data;
	union power_supply_propval state;
	int ret = -EINVAL;

	psy = power_supply_get_by_name(pm860x_supplied_to[0]);
	if (!psy)
		goto out;
	ret = psy->get_property(psy, POWER_SUPPLY_PROP_HEALTH, &state);
	if (ret)
		goto out;

	mutex_lock(&info->lock);
	switch (state.intval) {
	case POWER_SUPPLY_HEALTH_GOOD:
		info->allowed = 1;
		break;
	case POWER_SUPPLY_HEALTH_OVERHEAT:
	case POWER_SUPPLY_HEALTH_COLD:
		info->allowed = 0;
		break;
	case POWER_SUPPLY_HEALTH_DEAD:
		info->allowed = 0;
		break;
	}
	dev_dbg(info->dev, "%s, Allowed:%d\n", __func__, info->allowed);
	mutex_unlock(&info->lock);

	set_charging_fsm(info);
out:
	return IRQ_HANDLED;
}

static irqreturn_t pm860x_exception_handler(int irq, void *data)
{
	struct pm860x_charger_info *info = data;

	mutex_lock(&info->lock);
	info->allowed = 0;
	mutex_unlock(&info->lock);
	dev_dbg(info->dev, "%s, irq:%d\n", __func__, irq);

	set_charging_fsm(info);
	return IRQ_HANDLED;
}

static irqreturn_t pm860x_done_handler(int irq, void *data)
{
	struct pm860x_charger_info *info = data;
	int ret;

	mdelay(5);/* delay 5ms to make sure read the correct status of CHG_DET*/
	mutex_lock(&info->lock);
#if 1
	struct power_supply *psy;
	union power_supply_propval udata;
	int vbatt, ibatt, vchg, vsys;

	psy = power_supply_get_by_name(pm860x_supplied_to[0]);
	if (!psy)
		goto out;
	ret = psy->get_property(psy, POWER_SUPPLY_PROP_VOLTAGE_NOW, &udata);
	if (ret)
		goto out;
	vbatt = udata.intval / 1000;

	ret = psy->get_property(psy, POWER_SUPPLY_PROP_CURRENT_NOW, &udata);
	if (ret) {
		goto out;
	}
	ibatt = udata.intval;

	measure_vchg(info, &vchg);
	measure_vsys(info, &vsys);

	dev_dbg(info->dev, "%s, vbatt :%d, ibatt :%d, vchg : %d, vsys : %d.\n", __func__, vbatt, ibatt, vchg, vsys);
#endif
	if (info->state == FSM_PRECHARGE) {
		info->allowed = 1;
	} else {
		info->allowed = 0;
		/* CHG_DONE interrupt is faster than CHG_DET interrupt when
		 * plug in/out usb, So we can not rely on info->online, we
		 * need check pm8607 status register to check usb is online
		 * or not, then we can decide it is real charge done
		   automatically or it is triggered by usb plug out;
		   */
		ret = pm860x_reg_read(info->i2c, PM8607_STATUS_2);
		if (ret & STATUS2_CHG)
			pm860x_battery_update_soc();
	}
out:
	dev_dbg(info->dev, "%s, Allowed : %d\n", __func__, info->allowed);
	mutex_unlock(&info->lock);

	set_charging_fsm(info);
	return IRQ_HANDLED;
}

static irqreturn_t pm860x_vbattery_handler(int irq, void *data)
{
	struct pm860x_charger_info *info = data;
	int vbatt, ret;
	struct power_supply *psy;
	union power_supply_propval udata;

	psy = power_supply_get_by_name(pm860x_supplied_to[0]);
	if (!psy)
		goto out;
#if 0
	ret = psy->get_property(psy, POWER_SUPPLY_PROP_VOLTAGE_AVG, &udata);/* here using ocv value. */
#else
	ret = psy->get_property(psy, POWER_SUPPLY_PROP_VOLTAGE_NOW, &udata);
#endif
	if (ret)
		goto out;
	vbatt = udata.intval / 1000;

	if(vbatt > CHARGE_THRESHOLD){
		dev_dbg(info->dev, "%s, vbatt normal: %dmv\n", __func__, vbatt);
		return IRQ_HANDLED;
	}

	mutex_lock(&info->lock);

	set_vbatt_threshold(info, 0, 0);

	if (info->present && info->online)
		info->allowed = 1;
	else
		info->allowed = 0;
	mutex_unlock(&info->lock);
	dev_dbg(info->dev, "%s, Allowed:%d,vbatt = %dmv\n", __func__, info->allowed, vbatt);

	set_charging_fsm(info);
out:
	return IRQ_HANDLED;
}

static irqreturn_t pm860x_vchg_handler(int irq, void *data)
{
	struct pm860x_charger_info *info = data;
	int vchg = 0,status=0;

	measure_vchg(info,&vchg);

	mutex_lock(&info->lock);
	if (info->present) {
		if (!info->online) {
			/* check if over-temp on pm8606 or not*/
			status = pm860x_reg_read(info->i2c_8606, PM8606_FLAGS);
			if(status&OVER_TEMP_FLAG){
				pm860x_set_bits(info->i2c_8606, PM8606_FLAGS,
					OVER_TEMP_FLAG, OVER_TEMP_FLAG);/*clear flag*/
				pm860x_set_bits(info->i2c_8606, PM8606_VSYS,
					OVTEMP_AUTORECOVER, OVTEMP_AUTORECOVER);
				dev_dbg(info->dev, "%s,pm8606 over-temp occure\n",__func__);
			}
		}

		if(vchg > VCHG_NORMAL_CHECK){
			set_vchg_threshold(info, VCHG_OVP_LOW, 0);
			info->allowed = 0;
			dev_dbg(info->dev, "%s,pm8607 over-vchg occure,vchg = %dmv\n",
				__func__,vchg);
		} else if(vchg < VCHG_OVP_LOW){
#if DEBUG
			if(vchg > VCHG_NORMAL_LOW){
				dev_dbg(info->dev, "%s,pm8607 vchg normal!!,vchg = %dmv\n", __func__, vchg);
				mutex_unlock(&info->lock);
				return IRQ_HANDLED;
           }
#endif
			set_vchg_threshold(info, VCHG_NORMAL_LOW, VCHG_NORMAL_HIGH);
			info->allowed = 1;
			dev_dbg(info->dev, "%s,pm8607 over-vchg recover,vchg = %dmv\n",
				__func__,vchg);
		}
	}
	mutex_unlock(&info->lock);

	dev_dbg(info->dev, "%s, Allowed:%d\n", __func__, info->allowed);

	set_charging_fsm(info);

	return IRQ_HANDLED;
}

static ssize_t stop_charging(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	struct pm860x_charger_info *info = dev_get_drvdata(dev);
	int disable;

	sscanf(buf, "%d", &disable);
	if (disable && info) {
		dev_dbg(info->dev, "stop charging by manual\n");

		mutex_lock(&info->lock);
		info->allowed = 0;
		mutex_unlock(&info->lock);
		set_charging_fsm(info);
	}
	return strnlen(buf, PAGE_SIZE);
}

static DEVICE_ATTR(stop, S_IWUSR, NULL, stop_charging);

static int pm860x_usb_get_prop(struct power_supply *psy,
			       enum power_supply_property psp,
			       union power_supply_propval *val)
{
	struct pm860x_charger_info *info = dev_get_drvdata(psy->dev->parent);
	int type_usb = 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		if (info->charge_type == USB_CHARGER)
			type_usb = 1;
		val->intval = info->online && type_usb;
		break;
	default:
		return -ENODEV;
	}
	return 0;
}

static enum power_supply_property pm860x_usb_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static int pm860x_ac_get_prop(struct power_supply *psy,
			       enum power_supply_property psp,
			       union power_supply_propval *val)
{
	struct pm860x_charger_info *info = dev_get_drvdata(psy->dev->parent);
	int type_ac = 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		switch (info->charge_type) {
		case AC_STANDARD_CHARGER:
		case AC_OTHER_CHARGER:
			type_ac = 1;
			break;
		}
		val->intval = info->online && type_ac;
		break;
	default:
		return -ENODEV;
	}
	return 0;
}

static enum power_supply_property pm860x_ac_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static int measure_vchg(struct pm860x_charger_info *info, int *data)
{
	unsigned char buf[2];
	int ret=0;

	ret = pm860x_bulk_read(info->i2c, PM8607_VCHG_MEAS1, 2, buf);
	if (ret < 0)
		return ret;

	*data = ((buf[0] & 0xff) << 4) | (buf[1] & 0x0f);
	/* V_BATT_MEAS(mV) = value * 5 * 1.8 * 1000 / (2^12) */
	*data = ((*data & 0xfff) * 9 * 125) >> 9;

	dev_dbg(info->dev, "%s,vchg:%dmv\n",__func__,*data);

	return ret;
}

static int measure_vsys(struct pm860x_charger_info *info, int *data)
{
	unsigned char buf[2];
	int ret=0;

	ret = pm860x_bulk_read(info->i2c, PM8607_VSYS_MEAS1, 2, buf);
	if (ret < 0)
		return ret;

	*data = ((buf[0] & 0xff) << 4) | (buf[1] & 0x0f);
	/* V_BATT_MEAS(mV) = value * 5 * 1.8 * 1000 / (2^12) */
	*data = ((*data & 0xfff) * 9 * 125) >> 9;

	dev_dbg(info->dev, "%s,vsys:%dmv\n",__func__,*data);

	return ret;
}

static void set_vchg_threshold(struct pm860x_charger_info *info,
				int min, int max)
{
	int data;

	/* (tmp << 8) * / 5 / 1800 */
	if (min <= 0)
		data = 0;
	else
		data = (min << 5) / 1125;
	pm860x_reg_write(info->i2c, PM8607_VCHG_LOWTH, data);
//	dev_dbg(info->dev, "VCHG_LOWTH:%dmv,0x%x\n", min,data);

	if (max <= 0)
		data = 0xff;
	else
		data = (max << 5) / 1125;
	pm860x_reg_write(info->i2c, PM8607_VCHG_HIGHTH, data);
//	dev_dbg(info->dev, "VCHG_HIGHTH:%dmv,0x%x\n", max,data);

}

static void set_vbatt_threshold(struct pm860x_charger_info *info,
				int min, int max)
{
	int data;

	/* (tmp << 8) * 3 / 1800 */
	if (min <= 0)
		data = 0;
	else
		data = (min << 5) / 675;
	pm860x_reg_write(info->i2c, PM8607_VBAT_LOWTH, data);
	dev_dbg(info->dev, "VBAT Min:%dmv, LOWTH:0x%x\n", min, data);

	if (max <= 0)
		data = 0xff;
	else
		data = (max << 5) / 675;
	pm860x_reg_write(info->i2c, PM8607_VBAT_HIGHTH, data);
	dev_dbg(info->dev, "VBAT Max:%dmv, HIGHTH:0x%x\n", max, data);
}

static int start_precharge(struct pm860x_charger_info *info)
{
	int ret;

	dev_dbg(info->dev, "Start Pre-charging!\n");
	set_vbatt_threshold(info, 0, 0);

	ret = pm860x_reg_write(info->i2c_8606, PM8606_PREREGULATORA,
				   PREREG1_630MA | PREREG1_VSYS_4_5V);
	if (ret < 0)
		goto out;
	/* stop charging */
	ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL1, 3,
			      CC1_MODE_OFF);
	if (ret < 0)
		goto out;
	/* set 270 minutes timeout */
	ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL3, (0xf << 4),
			      CC3_270MIN_TIMEOUT);
	if (ret < 0)
		goto out;
	/* set precharge current, termination voltage, IBAT & TBAT monitor */
	ret = pm860x_reg_write(info->i2c, PM8607_CHG_CTRL4,
			       CC4_IPRE_40MA | CC4_VPCHG_3_2V | CC4_IFCHG_MON_EN
			       | CC4_BTEMP_MON_EN);
	if (ret < 0)
		goto out;
	ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL7,
			      CC7_BAT_REM_EN | CC7_IFSM_EN,
			      CC7_BAT_REM_EN | CC7_IFSM_EN);
	if (ret < 0)
		goto out;
	/* trigger precharge */
	ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL1, 3,
			      CC1_MODE_PRECHARGE);
out:
	return ret;
}

static int start_fastcharge(struct pm860x_charger_info *info)
{
	unsigned char buf[6];
	int ret;

	dev_dbg(info->dev, "Start Fast-charging!\n");
	set_vchg_threshold(info, VCHG_NORMAL_LOW, VCHG_NORMAL_HIGH);
	pm860x_calc_resistor();

	ret = pm860x_reg_write(info->i2c_8606, PM8606_PREREGULATORA,
				   PREREG1_630MA | PREREG1_VSYS_4_5V);
	if (ret < 0)
		goto out;
	/* set fastcharge termination current & voltage, disable charging */
	ret = pm860x_reg_write(info->i2c, PM8607_CHG_CTRL1,
				   CC1_MODE_OFF | CC1_ITERM_20MA | CC1_VFCHG_4_2V);
/*                   CC1_MODE_OFF | CC1_ITERM_60MA | CC1_VFCHG_4_2V);*/
	if (ret < 0)
		goto out;
	switch (info->charge_type) {
	case USB_CHARGER:
	case AC_STANDARD_CHARGER:
		ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL2, 0x1f,
				      CC2_ICHG_500MA);
		break;
	case AC_OTHER_CHARGER:
		ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL2, 0x1f,
				      CC2_ICHG_500MA);
/*                      CC2_ICHG_1000MA);*/
		break;
	default:
		ret = -EINVAL;
		break;
	}
	if (ret < 0)
		goto out;
	/* set 270 minutes timeout */
	ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL3, (0xf << 4),
			      CC3_270MIN_TIMEOUT);
	if (ret < 0)
		goto out;
	/* set IBAT & TBAT monitor */
	ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL4,
			      CC4_IFCHG_MON_EN | CC4_BTEMP_MON_EN,
			      CC4_IFCHG_MON_EN | CC4_BTEMP_MON_EN);
	if (ret < 0)
		goto out;
	ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL6,
			      CC6_BAT_OV_EN | CC6_BAT_UV_EN | CC6_UV_VBAT_SET,
			      CC6_BAT_OV_EN | CC6_BAT_UV_EN | CC6_UV_VBAT_SET);
	if (ret < 0)
		goto out;
	ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL7,
			      CC7_BAT_REM_EN | CC7_IFSM_EN,
			      CC7_BAT_REM_EN | CC7_IFSM_EN);
	if (ret < 0)
		goto out;
	/*hw fix workaround: disable BC_SHORT by setting in testpage,
	only occur before sanremo C1*/
	if((info->chip->chip_version <= PM8607_CHIP_C1) && !info->bc_short){
		info->bc_short = 1;/* disable bc_short mechanism*/
		buf[0] = buf[2] = 0x0;
		buf[1] = 0x60;
		buf[3] = 0xff;
		buf[4] = 0x9f;
		buf[5] = 0xfd;
		pm860x_page_bulk_write(info->i2c, 0xC8, 6, buf);
		pm860x_page_reg_write(info->i2c, 0xCF, 0x02);
	}
	/* trigger fastcharge */
	ret = pm860x_set_bits(info->i2c, PM8607_CHG_CTRL1, 3,
			      CC1_MODE_FASTCHARGE);
out:
	return ret;
}

static int stop_charge(struct pm860x_charger_info *info, int vbatt)
{
	dev_dbg(info->dev, "Stop charging!\n");
	pm860x_set_bits(info->i2c, PM8607_CHG_CTRL1, 3, CC1_MODE_OFF);
	if (vbatt > CHARGE_THRESHOLD && info->online) {
		set_vbatt_threshold(info, CHARGE_THRESHOLD, 0);
	}
	/*hw fix workaround: enable bc_short again after fast charge finished*/
	if((info->chip->chip_version <= PM8607_CHIP_C1) && info->bc_short){
		info->bc_short = 0;/* enable bc_short mechanism*/
		msleep(2);
		pm860x_page_reg_write(info->i2c, 0xCF, 0x0);
	}
	return 0;
}

static int power_off_notification(struct pm860x_charger_info *info)
{
	dev_dbg(info->dev, "Power-off notification!\n");
	return 0;
}

static int set_charging_fsm(struct pm860x_charger_info *info)
{
	struct power_supply *psy;
	union power_supply_propval data;
	unsigned char fsm_state[][16] = { "init", "discharge", "precharge",
					  "fastcharge", };
	int ret = -EINVAL;
	int vbatt;

	psy = power_supply_get_by_name(pm860x_supplied_to[0]);
	if (!psy)
		goto out;
#if 1
	ret = psy->get_property(psy, POWER_SUPPLY_PROP_VOLTAGE_AVG, &data);/* here using ocv value. */
#else
	ret = psy->get_property(psy, POWER_SUPPLY_PROP_VOLTAGE_NOW, &data);
#endif
	if (ret)
		goto out;
	vbatt = data.intval / 1000;

	ret = psy->get_property(psy, POWER_SUPPLY_PROP_PRESENT, &data);
	if (ret) {
		goto out;
	}
	mutex_lock(&info->lock);
	info->present = data.intval;

	dev_dbg(info->dev, "Entering FSM:%s, Charger:%s, Battery:%s, "
		"Allowed:%d\n",
		&fsm_state[info->state][0],
		(info->online) ? "online" : "N/A",
		(info->present) ? "present" : "N/A", info->allowed);
	dev_dbg(info->dev, "set_charging_fsm:vbatt:%d(mV)\n", vbatt);

	switch (info->state) {
	case FSM_INIT:
		if (info->online && info->present && info->allowed) {
			if (vbatt < PRECHARGE_THRESHOLD) {
				info->state = FSM_PRECHARGE;
				start_precharge(info);
			} else if (vbatt > DISCHARGE_THRESHOLD) {
				info->state = FSM_DISCHARGE;
				stop_charge(info, vbatt);
			} else if (vbatt < DISCHARGE_THRESHOLD) {
				info->state = FSM_FASTCHARGE;
				start_fastcharge(info);
			}
		} else {
			if (vbatt < POWEROFF_THRESHOLD) {
				power_off_notification(info);
			} else {
				info->state = FSM_DISCHARGE;
				stop_charge(info, vbatt);
			}
		}
		break;
	case FSM_PRECHARGE:
		if (info->online && info->present && info->allowed) {
			if (vbatt > PRECHARGE_THRESHOLD) {
				info->state = FSM_FASTCHARGE;
				start_fastcharge(info);
			}
		} else {
			info->state = FSM_DISCHARGE;
			stop_charge(info, vbatt);
		}
		break;
	case FSM_FASTCHARGE:
		if (info->online && info->present && info->allowed) {
			if (vbatt < PRECHARGE_THRESHOLD) {
				info->state = FSM_PRECHARGE;
				start_precharge(info);
			} else if (vbatt > DISCHARGE_THRESHOLD) {
				pr_info("FSM_FASTCHARGE:(vbatt > DISCHARGE_THRESHOLD)->stop chg?no!\n");
			}
		} else {
			info->state = FSM_DISCHARGE;
			stop_charge(info, vbatt);
		}
		break;
	case FSM_DISCHARGE:
		if (info->online && info->present && info->allowed) {
			if (vbatt < PRECHARGE_THRESHOLD) {
				info->state = FSM_PRECHARGE;
				start_precharge(info);
			} else if (vbatt < DISCHARGE_THRESHOLD) {
				info->state = FSM_FASTCHARGE;
				start_fastcharge(info);
			}
		} else {
			if (vbatt < POWEROFF_THRESHOLD) {
				power_off_notification(info);
			} else if (vbatt > CHARGE_THRESHOLD && info->online)
				set_vbatt_threshold(info, CHARGE_THRESHOLD, 0);
		}
		break;
	default:
		dev_warn(info->dev, "FSM meets wrong state:%d\n", info->state);
		break;
	}
	dev_dbg(info->dev, "Out FSM:%s, Charger:%s, Battery:%s, Allowed:%d\n",
		&fsm_state[info->state][0],
		(info->online) ? "online" : "N/A",
		(info->present) ? "present" : "N/A", info->allowed);
	mutex_unlock(&info->lock);

	return 0;
out:
	return ret;
}

static int pm860x_init_charger(struct pm860x_charger_info *info)
{
	int ret;

	mutex_lock(&info->lock);
	info->state = FSM_INIT;
	ret = pm860x_reg_read(info->i2c, PM8607_STATUS_2);
	if (ret < 0)
		goto out;
	if (ret & STATUS2_CHG) {
		info->online = 1;
		info->allowed = 1;
	} else {
		info->online = 0;
		info->allowed = 0;
	}
	info->charge_type = USB_CHARGER;
	mutex_unlock(&info->lock);

	set_charging_fsm(info);
	return 0;
out:
	return ret;
}

#ifdef	CONFIG_PROC_FS
#define PM860X_POWER_REG_NUM		0xef
#define	PM860X_POWER_PROC_FILE	"driver/pm860x_charger"
static struct proc_dir_entry *pm860x_power_proc_file;
static int index;

static ssize_t pm860x_power_proc_read(char *page, char **start, off_t off,
		int count, int *eof, void *data)
{
	u8 reg_val;
	int len=0;
	struct pm860x_charger_info *info = ginfo;

	if (index == 0xffff) {
		int i;
		printk(KERN_INFO "pm8607:sanremo reg dump\n");
		for (i = 0; i < PM860X_POWER_REG_NUM; i++) {
			reg_val = pm860x_reg_read(info->i2c, i);
			printk(KERN_INFO "[0x%02x]=0x%02x\n", i, reg_val);
		}
		return 0;
	}
	if((index < 0) || (index > PM860X_POWER_REG_NUM))
		return 0;
	reg_val = pm860x_reg_read(info->i2c, index);
	len = sprintf(page, "0x%x:0x%x\n",index,reg_val);

	return len;
}

static ssize_t pm860x_power_proc_write(struct file *filp,
				       const char *buff, size_t len,
				       loff_t * off)
{
	u8 reg_val;
	char messages[256], vol[256];
	int value = 0;
	u8 temp1,temp2,temp3;
	struct pm860x_charger_info *info = ginfo;

	if (len > 256)
		len = 256;

	if (copy_from_user(messages, buff, len))
		return -EFAULT;

	if ('-' == messages[0]) {
		/* set the register index */
		memcpy(vol, messages + 1, len - 1);
		index = (int) simple_strtoul(vol, NULL, 16);
		printk("index=0x%x\n", index);
	} else if('d' == messages[0]){
		temp1 = pm860x_page_reg_read(info->i2c,0xD4);
		temp2 = pm860x_page_reg_read(info->i2c,0xD5);
		temp3 = pm860x_page_reg_read(info->i2c,0xD7);
		value = temp1 + (temp2<<8) + (temp3<<16);
		printk("d4=0x%x, d5=0x%x,d7=0x%x value:0x%x,value:%d\n",
				temp1,temp2,temp3,value,value);
	} else {
		/* set the register value */
		reg_val = (int) simple_strtoul(messages, NULL, 16);
		pm860x_reg_write(info->i2c, index, reg_val & 0xFF);
	}

	return len;
}

static void create_pm860x_power_proc_file(void)
{
	pm860x_power_proc_file =
	    create_proc_entry(PM860X_POWER_PROC_FILE, 0644, NULL);
	if (pm860x_power_proc_file) {
		pm860x_power_proc_file->read_proc =
			pm860x_power_proc_read;
		pm860x_power_proc_file->write_proc =
			(write_proc_t  *)pm860x_power_proc_write;
	} else
		printk(KERN_INFO "proc file create failed!\n");
}

static void remove_pm860x_power_proc_file(void)
{
	extern struct proc_dir_entry proc_root;
	remove_proc_entry(PM860X_POWER_PROC_FILE, &proc_root);
}
#endif

static __devinit int pm860x_charger_probe(struct platform_device *pdev)
{
	struct pm860x_chip *chip = dev_get_drvdata(pdev->dev.parent);
	struct pm860x_charger_info *info;
	int ret, i, j, count;

	info = kzalloc(sizeof(struct pm860x_charger_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	ret = device_create_file(&pdev->dev, &dev_attr_stop);
	if (ret < 0)
		goto out;

	count = pdev->num_resources;
	for (i = 0, j = 0; i < count; i++) {
		info->irq[j] = platform_get_irq(pdev, i);
		if (info->irq[j] < 0)
			continue;
		j++;
	}
	ginfo = info;
	info->irq_nums = j;

	info->chip = chip;
	info->i2c = (chip->id == CHIP_PM8607) ? chip->client : chip->companion;
	info->i2c_8606 = (chip->id == CHIP_PM8607) ? chip->companion
			: chip->client;
	if (!info->i2c_8606) {
		dev_err(&pdev->dev, "Missed I2C address of 88PM8606!\n");
		ret = -EINVAL;
		goto out_dev;
	}
	info->dev = &pdev->dev;

	/* set init value for the case we are not using battery*/
	set_vchg_threshold(info, VCHG_NORMAL_LOW, VCHG_OVP_LOW);

#if !defined(CONFIG_USB_VBUS_88PM860X)
	ret = request_threaded_irq(info->irq[0], NULL,
				   pm860x_charger_handler,
				   IRQF_ONESHOT, "usb supply detect", info);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to request IRQ: #%d: %d\n",
			info->irq[0], ret);
		goto out_dev;
	}
#endif
	ret = request_threaded_irq(info->irq[1], NULL,
				   pm860x_done_handler,
				   IRQF_ONESHOT, "charge done", info);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to request IRQ: #%d: %d\n",
			info->irq[1], ret);
		goto out_irq1;
	}
	ret = request_threaded_irq(info->irq[2], NULL,
				   pm860x_exception_handler,
				   IRQF_ONESHOT, "charge timeout", info);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to request IRQ: #%d: %d\n",
			info->irq[2], ret);
		goto out_irq2;
	}
	ret = request_threaded_irq(info->irq[3], NULL,
				   pm860x_exception_handler,
				   IRQF_ONESHOT, "charge fault", info);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to request IRQ: #%d: %d\n",
			info->irq[3], ret);
		goto out_irq3;
	}
	ret = request_threaded_irq(info->irq[4], NULL,
				   pm860x_temp_handler,
				   IRQF_ONESHOT, "temperature", info);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to request IRQ: #%d: %d\n",
			info->irq[4], ret);
		goto out_irq4;
	}
	ret = request_threaded_irq(info->irq[5], NULL,
				   pm860x_vbattery_handler,
				   IRQF_ONESHOT, "vbatt", info);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to request IRQ: #%d: %d\n",
			info->irq[5], ret);
		goto out_irq5;
	}
	ret = request_threaded_irq(info->irq[6], NULL,
				   pm860x_vchg_handler,
				   IRQF_ONESHOT, "vchg", info);
	if (ret < 0) {
		dev_err(chip->dev, "Failed to request IRQ: #%d: %d\n",
			info->irq[6], ret);
		goto out_irq6;
	}
	if (info->irq_nums <= 6) {
		dev_err(chip->dev, "IRQ numbers aren't matched\n");
		goto out_nums;
	}

	mutex_init(&info->lock);
	platform_set_drvdata(pdev, info);

	info->usb.name = "usb";
	info->usb.type = POWER_SUPPLY_TYPE_USB;
	info->usb.supplied_to = pm860x_supplied_to;
	info->usb.num_supplicants = ARRAY_SIZE(pm860x_supplied_to);
	info->usb.properties = pm860x_usb_props;
	info->usb.num_properties = ARRAY_SIZE(pm860x_usb_props);
	info->usb.get_property = pm860x_usb_get_prop;
	ret = power_supply_register(&pdev->dev, &info->usb);
	if (ret)
		goto out_nums;

	info->ac.name = "ac";
	info->ac.type = POWER_SUPPLY_TYPE_MAINS;
	info->ac.supplied_to = pm860x_supplied_to;
	info->ac.num_supplicants = ARRAY_SIZE(pm860x_supplied_to);
	info->ac.properties = pm860x_ac_props;
	info->ac.num_properties = ARRAY_SIZE(pm860x_ac_props);
	info->ac.get_property = pm860x_ac_get_prop;
	ret = power_supply_register(&pdev->dev, &info->ac);
	if (ret)
		goto out_nums;

	pm860x_init_charger(info);
	INIT_WORK(&info->vbus_work, pm860x_vbus_work);
	device_init_wakeup(&pdev->dev, 1);
#ifdef	CONFIG_PROC_FS
	create_pm860x_power_proc_file();
#endif
	return 0;

out_nums:
	free_irq(info->irq[6], info);
out_irq6:
	free_irq(info->irq[5], info);
out_irq5:
	free_irq(info->irq[4], info);
out_irq4:
	free_irq(info->irq[3], info);
out_irq3:
	free_irq(info->irq[2], info);
out_irq2:
	free_irq(info->irq[1], info);
out_irq1:
#if !defined(CONFIG_USB_VBUS_88PM860X)
	free_irq(info->irq[0], info);
#endif
out_dev:
	device_remove_file(&pdev->dev, &dev_attr_stop);
out:
	kfree(info);
	return ret;
}

static int __devexit pm860x_charger_remove(struct platform_device *pdev)
{
	struct pm860x_charger_info *info = platform_get_drvdata(pdev);
	int i;

	cancel_work_sync(&info->vbus_work);
	flush_workqueue(info->chip->monitor_wqueue);
	platform_set_drvdata(pdev, NULL);
	power_supply_unregister(&info->usb);
#if !defined(CONFIG_USB_VBUS_88PM860X)
	free_irq(info->irq[0], info);
#endif
	for (i = 1; i < info->irq_nums; i++)
		free_irq(info->irq[i], info);
	device_remove_file(info->dev, &dev_attr_stop);
#ifdef	CONFIG_PROC_FS
	remove_pm860x_power_proc_file();
#endif
	kfree(info);
	return 0;
}

#if defined(CONFIG_PM) && !defined(CONFIG_PXA95x_SUSPEND)
static int pm860x_charger_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct pm860x_charger_info *info = dev_get_drvdata(dev);
	int irq, i;

	mutex_lock(&info->lock);
	info->allowed = 0;
	mutex_unlock(&info->lock);

	set_charging_fsm(info);

	if (device_may_wakeup(dev)) {
		enable_irq_wake(info->chip->core_irq);
		for (i = 0; i < info->irq_nums; i++) {
#if defined(CONFIG_USB_VBUS_88PM860X)
			/* this IRQ should be handled in VBUS driver */
			if (i == 0)
				continue;
#endif
			irq = platform_get_irq(pdev, i);
			enable_irq_wake(irq);
		}
	}
	set_vbatt_threshold(info, POWEROFF_THRESHOLD, 0);
	return 0;
}

static int pm860x_charger_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct pm860x_charger_info *info = dev_get_drvdata(dev);
	int irq, i;

	if (device_may_wakeup(dev)) {
		disable_irq_wake(info->chip->core_irq);
		for (i = 0; i < info->irq_nums; i++) {
#if defined(CONFIG_USB_VBUS_88PM860X)
			if (i == 0)
				continue;
#endif
			irq = platform_get_irq(pdev, i);
			disable_irq_wake(irq);
		}
	}
	set_vbatt_threshold(info, 0, 0);
	return 0;
}

static struct dev_pm_ops pm860x_charger_pm_ops = {
	.suspend	= pm860x_charger_suspend,
	.resume		= pm860x_charger_resume,
};
#endif

static struct platform_driver pm860x_charger_driver = {
	.driver		= {
		.name	= "88pm860x-charger",
		.owner	= THIS_MODULE,
#if defined(CONFIG_PM) && !defined(CONFIG_PXA95x_SUSPEND)
		.pm	= &pm860x_charger_pm_ops,
#endif
	},
	.probe		= pm860x_charger_probe,
	.remove		= __devexit_p(pm860x_charger_remove),
};

static int __init pm860x_charger_init(void)
{
	return platform_driver_register(&pm860x_charger_driver);
}
module_init(pm860x_charger_init);

static void __exit pm860x_charger_exit(void)
{
	platform_driver_unregister(&pm860x_charger_driver);
}
module_exit(pm860x_charger_exit);

MODULE_DESCRIPTION("Marvell 88PM860x Battery driver");
MODULE_LICENSE("GPL");
