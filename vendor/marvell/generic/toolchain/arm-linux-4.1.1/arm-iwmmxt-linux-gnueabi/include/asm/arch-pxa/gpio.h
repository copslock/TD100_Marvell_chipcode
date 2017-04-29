#ifndef _MHN_GPIO_H
#define _MHN_GPIO_H

#include <linux/types.h>
/* platform specific GPIO settings, will be moved to corresponding driver */

#if	defined(CONFIG_CPU_MONAHANS_PL) || defined(CONFIG_CPU_MONAHANS_L)

#define	GPIO_GPIO_EXT0	     12
#define	GPIO_GPIO_EXT1	     13

/* Because the GPIO-expander for MMC_CD_0 can't work correctly,
 * We use MMC_CD_1 as MMC_CD_0 now
 */
#define GPIO_MMC_CD_0           128
#define GPIO_MMC_CD_1           129 
#define GPIO_MMC_WP_0_N         130
#define GPIO_MMC_WP_1_N         131
#define GPIO_PHONE_FLIPPED      132
#define GPIO_PHONE_CLOSED       133
#define GPIO_USB2_DETECT	134
#define GPIO_CF_CD_N		135
#define GPIO_CAMERA_LIGHT_EN    136
#define GPIO_IR_SHDN_N		137
#define GPIO_CAMERA_HI_PWDN     138
#define GPIO_CAMERA_LO_PWDN     139
#define	GPIO_UTMI_TEST_EN       140
#define	GPIO_UTMI_SWITCH        141
#define	GPIO_USB_OTG_EN         142
#define	GPIO_USB_OTG_SR         143

#ifdef CONFIG_CPU_MONAHANS_PL

#define GPIO_DEBUG_ETH_CS_N     4 
#define GPIO_DEBUG_ETH_INT      97 
#define GPIO_AC97_INT_N         36 
#define GPIO_CAMERA_STROBE_EN   2 
#define GPIO_MMC_CMD_0	        23 
#define GPIO_MMC_CMD_1	        9
#define GPIO_MMC2_CMD	        29
#define GPIO_CIR_ON_PWM		15

#else

#define GPIO_DEBUG_ETH_CS_N     2
#define GPIO_DEBUG_ETH_INT      99 
#define GPIO_AC97_INT_N         26 
#define GPIO_CAMERA_STROBE_EN   0
#define GPIO_MMC_CMD_0	        8 
#define GPIO_MMC_CMD_1	        15
#define GPIO_MMC2_CMD	        14
#define GPIO_CIR_ON_PWM		16

#endif

#else

#define GPIO_MMC_CD_0           1 
#define GPIO_DEBUG_ETH_CS_N     3 
#define GPIO_MMC_CD_1           4 
#define GPIO_MMC_WP_0_N         5 
#define GPIO_MMC_WP_1_N         6 
#define GPIO_PHONE_CLOSED       7 
#define GPIO_PHONE_FLIPPED      8 
#define GPIO_DEBUG_ETH_INT      9 
#define GPIO_AC97_INT_N        15
#define GPIO_CAMERA_STROBE_EN  16
#define GPIO_CAMERA_LIGHT_EN   17
#define GPIO_MMC_CMD_0         23
#define GPIO_MMC_CMD_1         31
#define GPIO_MMC2_CMD          29
#define GPIO_CFCD              30
#define GPIO_CFNIRQ            98
#define GPIO_CAMERA_HI_PWDN    102
#define GPIO_CAMERA_LO_PWDN    103

#define GPIO_IR_SHDN_N		97
#define GPIO_CIR_ON_PWM		13

#endif /* CONFIG_CPU_MONAHANS_PL */

#define GPIO_ID_START	0
#define GPIO_ID_END	127
#define GPIO_ID_MAX	GPIO_ID_END

#if defined(CONFIG_CPU_MONAHANS_PL) || defined(CONFIG_CPU_MONAHANS_L)

#define GPIO_EXP_START	(GPIO_ID_END + 1)
#define GPIO_EXP0_START	(GPIO_ID_END + 1)
#define GPIO_EXP0_HALF	(GPIO_EXP0_START + 8)
#define GPIO_EXP0_END	(GPIO_EXP0_START + 16)
#define GPIO_EXP1_START	(GPIO_ID_END + 16)
#define GPIO_EXP1_HALF	(GPIO_EXP1_START + 8)
#define GPIO_EXP1_END	(GPIO_EXP1_START + 16)
#define GPIO_EXP_END	(GPIO_EXP1_END)

#undef  GPIO_ID_MAX
#define GPIO_ID_MAX	(GPIO_EXP_END)

/* I2C address for GPIO Expander 0, 1 */
#define GPIO_EXP0_ADDR	(0x74)
#define GPIO_EXP1_ADDR	(0x75)

extern int gpio_exp_read (uint8_t exp_addr, uint8_t reg, uint8_t *val);
extern int gpio_exp_write(uint8_t exp_addr, uint8_t reg, uint8_t  val);
extern int gpio_exp_set_direction(int gpio_exp_id, int direction);
extern int gpio_exp_get_direction(int gpio_exp_id);
extern int gpio_exp_set_level(int gpio_exp_id, int level);
extern int gpio_exp_get_level(int gpio_exp_id);

#endif

#define GPIO_DIR_IN		0
#define GPIO_DIR_OUT		1
#define GPIO_LEVEL_LOW		0
#define GPIO_LEVEL_HIGH		1

extern int mhn_gpio_set_direction(int gpio_id, int dir);
extern int mhn_gpio_get_direction(int gpio_id);
extern int mhn_gpio_set_level(int gpio_id, int level);
extern int mhn_gpio_get_level(int gpio_id);
extern int mhn_gpio_set_rising_edge_detect(int gpio_id, int enable);
extern int mhn_gpio_get_rising_edge_detect(int gpio_id);
extern int mhn_gpio_set_falling_edge_detect(int gpio_id, int enable);
extern int mhn_gpio_get_falling_edge_detect(int gpio_id);
extern int mhn_gpio_get_edge_detect_status(int gpio_id);
extern int mhn_gpio_clear_edge_detect_status(int gpio_id);

extern void mhn_gpio_save(void);
extern void mhn_gpio_restore(void);

#endif /* _MHN_GPIO_H */
