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

#ifndef __BH1770GLC_HEADER__
#define __BH1770GLC_HEADER__

#include <linux/input.h>

#define BHCHIP_PS_INT_TIME     0x27 /* PS INTEGRATION TIME */
#define BHCHIP_ALS_CONTROL     0x80 /* ALS operation mode control */
#define BHCHIP_PS_CONTROL      0x81 /* PS operation mode control */
#define BHCHIP_I_LED           0x82 /* active LED and LED1, LED2 current */
#define BHCHIP_I_LED3          0x83 /* LED3 current setting */
#define BHCHIP_ALS_PS_MEAS     0x84 /* Forced mode trigger */
#define BHCHIP_PS_MEAS_RATE    0x85 /* PS meas. rate at stand alone mode */
#define BHCHIP_ALS_MEAS_RATE   0x86 /* ALS meas. rate at stand alone mode */
#define BHCHIP_PART_ID         0x8a /* Part number and revision ID */
#define BHCHIP_MANUFACT_ID     0x8b /* Manufacturerer ID */
#define BHCHIP_ALS_DATA_0      0x8c /* ALS DATA low byte */
#define BHCHIP_ALS_DATA_1      0x8d /* ALS DATA high byte */
#define BHCHIP_ALS_PS_STATUS   0x8e /* Measurement data and int status */
#define BHCHIP_PS_DATA_LED1    0x8f /* PS data from LED1 */
#define BHCHIP_PS_DATA_LED2    0x90 /* PS data from LED2 */
#define BHCHIP_PS_DATA_LED3    0x91 /* PS data from LED3 */
#define BHCHIP_INTERRUPT       0x92 /* Interrupt setting */
#define BHCHIP_PS_TH_LED1      0x93 /* PS interrupt threshold for LED1 */
#define BHCHIP_PS_TH_LED2      0x94 /* PS interrupt threshold for LED2 */
#define BHCHIP_PS_TH_LED3      0x95 /* PS interrupt threshold for LED3 */
#define BHCHIP_ALS_TH_UP_0     0x96 /* ALS upper threshold low byte */
#define BHCHIP_ALS_TH_UP_1     0x97 /* ALS upper threshold high byte */
#define BHCHIP_ALS_TH_LOW_0    0x98 /* ALS lower threshold low byte */
#define BHCHIP_ALS_TH_LOW_1    0x99 /* ALS lower threshold high byte */

/* MANUFACT_ID */
#define BHCHIP_MANUFACT_ROHM   0x01
#define BHCHIP_MANUFACT_OSRAM  0x03

/* PART_ID */
#define BHCHIP_PART            0x90
#define BHCHIP_PART_MASK       0xf0
#define BHCHIP_REV_MASK                0x0f
#define BHCHIP_REV_SHIFT       0
#define BHCHIP_REV_0           0x00

/* Operating modes for both */
#define BHCHIP_STANDBY         0x00
#define BHCHIP_FORCED          0x02
#define BHCHIP_STANDALONE      0x03

#define BHCHIP_PS_TRIG_MEAS    (1 << 0)
#define BHCHIP_ALS_TRIG_MEAS   (1 << 1)

/* Interrupt control */
#define BHCHIP_INT_OUTPUT_MODE (1 << 3) /* 0 = latched */
#define BHCHIP_INT_POLARITY    (1 << 2) /* 1 = active high */
#define BHCHIP_INT_ALS_ENA     (1 << 1)
#define BHCHIP_INT_PS_ENA      (1 << 0)

/* Interrupt status */
#define BHCHIP_INT_LED1_DATA   (1 << 0)
#define BHCHIP_INT_LED1_INT    (1 << 1)
#define BHCHIP_INT_LED2_DATA   (1 << 2)
#define BHCHIP_INT_LED2_INT    (1 << 3)
#define BHCHIP_INT_LED3_DATA   (1 << 4)
#define BHCHIP_INT_LED3_INT    (1 << 5)
#define BHCHIP_INT_LEDS_INT    ((1 << 1) | (1 << 3) | (1 << 5))
#define BHCHIP_INT_ALS_DATA    (1 << 6)
#define BHCHIP_INT_ALS_INT     (1 << 7)

#define BHCHIP_DISABLE         0
#define BHCHIP_ENABLE          1

 /* Following are milliseconds */
#define BHCHIP_ALS_DEFAULT_RATE        200
#define BHCHIP_PS_DEFAULT_RATE 100
#define BHCHIP_DEFAULT_DELAY        50
#define BHCHIP_PS_DEF_RATE_THRESH 200
#define BHCHIP_PS_INIT_DELAY    15
#define BHCHIP_STARTUP_DELAY   50

#define BHCHIP_ALS_RANGE       65535
#define BHCHIP_PS_RANGE                255
#define BHCHIP_CALIB_SCALER    1000
#define BHCHIP_ALS_NEUTRAL_CALIB_VALUE (1 * BHCHIP_CALIB_SCALER)
#define BHCHIP_PS_NEUTRAL_CALIB_VALUE  (1 * BHCHIP_CALIB_SCALER)
#define BHCHIP_ALS_DEF_SENS    10
#define BHCHIP_ALS_DEF_THRES   1000
#define BHCHIP_PS_DEF_THRES    100
#define BHCHIP_PS_DEF_INT_TIME    0x05

#define ALS_NBR_FORMAT         512
/* coef as decimal = ALS_COEF / *(ALS_NBR_FORMAT ^ 2) */
#define ALS_COEF               1536
/* Scaler coefficient at zero level */
#define ALS_ZERO_LEVEL         (ALS_NBR_FORMAT / 4)

#define PS_ABOVE_THRESHOLD     1
#define PS_BELOW_THRESHOLD     0

//#define BHCHIP_PS_CHANNELS 3
#define BHCHIP_PS_CHANNELS 1

#define SENSOR_NAME_LIGHT "bh1770glc_als"
#define SENSOR_NAME_PROXIMITY "bh1770glc_ps"


struct bh1770glc_chip {
       struct i2c_client               *client;
       struct bh1770glc_platform_data  *pdata;
       struct mutex                    mutex; /* avoid parallel access */
#if 0
       struct regulator_bulk_data      regs[2];
#endif
       struct mutex                    enable_mutex; /* avoid parallel access */
       bool                            int_mode_ps;
       bool                            int_mode_als;

       wait_queue_head_t               als_misc_wait; /* WQ for ALS part */
       wait_queue_head_t               ps_misc_wait; /* WQ for PS part */
       struct delayed_work             ps_work; /* For ps low threshold */
       struct delayed_work             als_work; /* For als low threshold */

       char                            chipname[10];
       u8                              revision;
 
       atomic_t delay;

	   struct input_dev *input_dev_als;
	   struct input_dev *input_dev_ps;
	   /* for HAL */
	   atomic_t enable_als;
	   atomic_t enable_ps;

       u32     als_calib;
       int     als_rate;
       int     als_mode;
       int     als_users;
       u16     als_data;
       u16     als_threshold_hi;
       u16     als_threshold_lo;
       u16     als_sens;
       loff_t  als_offset;

       loff_t  ps_offset;
       u32     ps_calib[BHCHIP_PS_CHANNELS];
       int     ps_rate;
       int     ps_rate_threshold;
       int     ps_mode;
       int     ps_int_time;
       int     ps_users;
       u8      ps_data[BHCHIP_PS_CHANNELS];
       u8      ps_thresholds[BHCHIP_PS_CHANNELS];
       u8      ps_leds[BHCHIP_PS_CHANNELS];
       u8      ps_channels; /* nbr of leds */
};

extern struct bh1770glc_chip *bh1770glc;

extern int bh1770glc_ps_mode(struct bh1770glc_chip *chip, int mode);
extern int bh1770glc_ps_init(struct bh1770glc_chip *chip);
extern int bh1770glc_ps_destroy(struct bh1770glc_chip *chip);
extern void bh1770glc_ps_interrupt_handler(struct bh1770glc_chip *chip,
                                       int status);

extern int bh1770glc_als_mode(struct bh1770glc_chip *chip, int mode);
extern int bh1770glc_als_init(struct bh1770glc_chip *chip);
extern int bh1770glc_als_destroy(struct bh1770glc_chip *chip);
extern void bh1770glc_als_interrupt_handler(struct bh1770glc_chip *chip,
                                       int status);
#endif
