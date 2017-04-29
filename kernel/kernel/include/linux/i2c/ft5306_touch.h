#ifndef __LINUX_I2C_FT5306_TOUCH_H
#define __LINUX_I2C_FT5306_TOUCH_H

/* linux/i2c/ft5306_touch.h */

struct ft5306_touch_platform_data {
	int(*power)(int);
	void(*reset)(void);
};

#endif
