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
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/mutex.h>
#include <linux/sfh7773.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include "sfh7773.h"

struct bh1770glc_chip *bh1770glc;

static const char reg_vcc[] = "Vcc";
static const char reg_vleds[] = "Vleds";

static int bh1770glc_detect(struct bh1770glc_chip *chip)
{
       struct i2c_client *client = chip->client;
       s32 ret;
       u8 manu;
       u8 part;

       ret = i2c_smbus_read_byte_data(client, BHCHIP_MANUFACT_ID);
       if (ret < 0)
               goto error;

       manu = (u8)ret;

       ret = i2c_smbus_read_byte_data(client, BHCHIP_PART_ID);
       if (ret < 0)
               goto error;
       part = (u8)ret;
       chip->revision = (part & BHCHIP_REV_MASK) >> BHCHIP_REV_SHIFT;

       if ((manu == BHCHIP_MANUFACT_ROHM) &&
           ((part & BHCHIP_PART_MASK)      == BHCHIP_PART)) {
               snprintf(chip->chipname, sizeof(chip->chipname), "BH1770GLC");
				printk("BH1770GLC be found.\n");
               return 0;
       }

       if ((manu == BHCHIP_MANUFACT_OSRAM) &&
           ((part & BHCHIP_PART_MASK)      == BHCHIP_PART)) {
               snprintf(chip->chipname, sizeof(chip->chipname), "SFH7770");
				printk("SFH7773 be found.\n");
				printk("DBJtech sfh7773 succeed\n");
               return 0;
       }

       ret = -ENODEV;
error:
       dev_dbg(&client->dev, "BH1770GLC or SFH7770 not found\n");

       return ret;
}

/* This is threaded irq handler */
static irqreturn_t bh1770glc_irq(int irq, void *data)
{
       struct bh1770glc_chip *chip = data;
       int status;

       status = i2c_smbus_read_byte_data(chip->client, BHCHIP_ALS_PS_STATUS);

       /* Acknowledge interrupt by reading this register */
       i2c_smbus_read_byte_data(chip->client, BHCHIP_INTERRUPT);

       bh1770glc_als_interrupt_handler(chip, status);
       bh1770glc_ps_interrupt_handler(chip, status);

       return IRQ_HANDLED;
}

static int __devinit bh1770glc_probe(struct i2c_client *client,
                               const struct i2c_device_id *id)
{
       struct bh1770glc_chip *chip;
       int err;
       int i;

       chip = kzalloc(sizeof *chip, GFP_KERNEL);
       if (!chip)
               return -ENOMEM;

       bh1770glc = chip;
#if 0
       init_waitqueue_head(&chip->ps_misc_wait);
       init_waitqueue_head(&chip->als_misc_wait);
#endif
       chip->client  = client;
       i2c_set_clientdata(client, chip);

       mutex_init(&chip->mutex);
       chip->pdata  = client->dev.platform_data;
       chip->als_calib = BHCHIP_ALS_NEUTRAL_CALIB_VALUE;
       chip->als_sens       = BHCHIP_ALS_DEF_SENS;
       for (i = 0; i < BHCHIP_PS_CHANNELS; i++)
               chip->ps_calib[i] = BHCHIP_PS_NEUTRAL_CALIB_VALUE;

       /*
        * Platform data contains led configuration and safety limits.
        * Too strong current can damage HW permanently.
        * Platform data filled with zeros causes minimum current.
        */
       if (chip->pdata == NULL) {
               dev_err(&client->dev, "platform data is mandatory\n");
               err = -EINVAL;
               goto fail1;
       }

       if (chip->pdata->setup_resources) {
               err = chip->pdata->setup_resources();
               if (err) {
                       err = -EINVAL;
                       goto fail1;
               }
       }

       switch (chip->pdata->leds) {
       case BH1770GLC_LED1:
               chip->ps_channels = 1;
               break;
       case BH1770GLC_LED12:
       case BH1770GLC_LED13:
               chip->ps_channels = 2;
               break;
       case BH1770GLC_LED123:
               chip->ps_channels = 3;
               break;
       default:
               err = -EINVAL;
               goto fail1;
       }

#if 0
       chip->regs[0].supply = reg_vcc;
       chip->regs[1].supply = reg_vleds;

       err = regulator_bulk_get(&client->dev,
                                ARRAY_SIZE(chip->regs), chip->regs);
       if (err < 0) {
               dev_err(&client->dev, "Cannot get regulators\n");
               goto fail1;
       }

       err = regulator_bulk_enable(ARRAY_SIZE(chip->regs), chip->regs);
       if (err < 0) {
               dev_err(&client->dev, "Cannot enable regulators\n");
               goto fail2;
       }
#endif
       err = bh1770glc_detect(chip);
       if (err < 0)
               goto fail3;

	   atomic_set(&chip->delay, BHCHIP_DEFAULT_DELAY);

       err = bh1770glc_als_init(chip);
       if (err < 0)
               goto fail3;

       err = bh1770glc_ps_init(chip);
       if (err < 0)
               goto fail4;

/*       printk("bh1770glc irq num %d\n", client->irq);*/

       err = request_threaded_irq(client->irq, NULL,
                               bh1770glc_irq,
                               IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
                               "bh1770glc", chip);
       if (err) {
               dev_err(&client->dev, "could not get IRQ %d\n",
                       client->irq);
               goto fail5;
       }
#if 0
       regulator_bulk_disable(ARRAY_SIZE(chip->regs), chip->regs);
#endif
   return err;
fail5:
       bh1770glc_ps_destroy(chip);
fail4:
       bh1770glc_als_destroy(chip);
fail3:
#if 0
       regulator_bulk_disable(ARRAY_SIZE(chip->regs), chip->regs);
fail2:
       regulator_bulk_free(ARRAY_SIZE(chip->regs), chip->regs);
#endif
fail1:
       kfree(chip);
       return err;
}

static int __devexit bh1770glc_remove(struct i2c_client *client)
{
       struct bh1770glc_chip *chip = i2c_get_clientdata(client);

       free_irq(client->irq, chip);

       if (chip->pdata && chip->pdata->release_resources)
               chip->pdata->release_resources();

       bh1770glc_als_destroy(chip);
       bh1770glc_ps_destroy(chip);
#if 0
       regulator_bulk_free(ARRAY_SIZE(chip->regs), chip->regs);
#endif
   kfree(chip);
       return 0;
}

static int bh1770glc_suspend(struct i2c_client *client, pm_message_t mesg)
{
       struct bh1770glc_chip *chip = i2c_get_clientdata(client);
       printk("bh1770glc suspend.\n");

       mutex_lock(&chip->mutex);
       if (chip->als_users)
               bh1770glc_als_mode(chip, BHCHIP_STANDBY);
       if (chip->ps_users)
               bh1770glc_ps_mode(chip, BHCHIP_STANDBY);
       mutex_unlock(&chip->mutex);
       return 0;
}

static int bh1770glc_resume(struct i2c_client *client)
{
       struct bh1770glc_chip *chip = i2c_get_clientdata(client);
	
       printk("bh1770glc resume.\n");
       mutex_lock(&chip->mutex);
       if (chip->als_users)
               bh1770glc_als_mode(chip, BHCHIP_STANDALONE);
       if (chip->ps_users)
               bh1770glc_ps_mode(chip, BHCHIP_STANDALONE);
       mutex_unlock(&chip->mutex);
       return 0;
}

static void bh1770glc_shutdown(struct i2c_client *client)
{
       struct bh1770glc_chip *chip = i2c_get_clientdata(client);

       bh1770glc_als_mode(chip, BHCHIP_STANDBY);
       bh1770glc_ps_mode(chip, BHCHIP_STANDBY);
}


static const struct i2c_device_id bh1770glc_id[] = {
       {"bh1770glc", 0 },
       {"SFH7770", 0 },
       {}
};

MODULE_DEVICE_TABLE(i2c, bh1770glc_id);

static struct i2c_driver bh1770glc_driver = {
       .driver  = {
               .name   = "bh1770glc",
               .owner  = THIS_MODULE,
       },
       .suspend  = bh1770glc_suspend,
       .shutdown = bh1770glc_shutdown,
       .resume   = bh1770glc_resume,
       .probe    = bh1770glc_probe,
       .remove   = __devexit_p(bh1770glc_remove),
       .id_table = bh1770glc_id,
};

static int __init bh1770glc_init(void)
{
       return i2c_add_driver(&bh1770glc_driver);
}

static void __exit bh1770glc_exit(void)
{
       i2c_del_driver(&bh1770glc_driver);
}

MODULE_DESCRIPTION("BH1770GLC combined ALS and proximity sensor");
MODULE_AUTHOR("Samu Onkalo, Nokia Corporation");
MODULE_LICENSE("GPL v2");

module_init(bh1770glc_init);
module_exit(bh1770glc_exit);
