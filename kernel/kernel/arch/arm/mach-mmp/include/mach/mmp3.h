#ifndef __ASM_MACH_MMP2_H
#define __ASM_MACH_MMP2_H

#include <linux/i2c.h>
#include <mach/devices.h>
#include <plat/i2c.h>
#include <plat/pxa3xx_nand.h>
#include <mach/pxa168fb.h>
#include <plat/sdhci.h>
#include <mach/cputype.h>
#include <mach/camera.h>
#include <plat/pxa27x_keypad.h>

extern struct pxa_device_desc mmp3_device_uart1;
extern struct pxa_device_desc mmp3_device_uart2;
extern struct pxa_device_desc mmp3_device_uart3;
extern struct pxa_device_desc mmp3_device_uart4;
extern struct pxa_device_desc mmp3_device_twsi1;
extern struct pxa_device_desc mmp3_device_twsi2;
extern struct pxa_device_desc mmp3_device_twsi3;
extern struct pxa_device_desc mmp3_device_twsi4;
extern struct pxa_device_desc mmp3_device_twsi5;
extern struct pxa_device_desc mmp3_device_twsi6;
extern struct pxa_device_desc mmp3_device_nand;
extern struct pxa_device_desc mmp3_device_sdh0;
extern struct pxa_device_desc mmp3_device_sdh1;
extern struct pxa_device_desc mmp3_device_sdh2;
extern struct pxa_device_desc mmp3_device_sdh3;
extern struct pxa_device_desc mmp3_device_camera0;
extern struct pxa_device_desc mmp3_device_camera1;
extern struct pxa_device_desc mmp3_device_pwm1;
extern struct pxa_device_desc mmp3_device_pwm2;
extern struct pxa_device_desc mmp3_device_pwm3;
extern struct pxa_device_desc mmp3_device_pwm4;
extern struct pxa_device_desc mmp3_device_fb;
extern struct pxa_device_desc mmp3_device_fb_ovly;
extern struct pxa_device_desc mmp3_device_sspa1;
extern struct pxa_device_desc mmp3_device_sspa2;
extern struct platform_device mmp3_device_audiosram;
extern struct platform_device mmp3_device_audiosram_A0;
extern struct platform_device mmp3_device_rtc;
extern struct pxa_device_desc mmp3_device_keypad;
extern struct platform_device pxa_device_u2o;
extern struct platform_device pxa_device_u2ootg;
extern struct platform_device pxa_device_u2oehci;
extern struct platform_device mmp3_hsic1_device;

static inline int mmp3_add_uart(int id)
{
	struct pxa_device_desc *d = NULL;

	switch (id) {
	case 1: d = &mmp3_device_uart1; break;
	case 2: d = &mmp3_device_uart2; break;
	case 3: d = &mmp3_device_uart3; break;
	case 4: d = &mmp3_device_uart4; break;
	default:
		return -EINVAL;
	}

	return pxa_register_device(d, NULL, 0);
}

static inline int mmp3_add_twsi(int id, struct i2c_pxa_platform_data *data,
				  struct i2c_board_info *info, unsigned size)
{
	struct pxa_device_desc *d = NULL;
	int ret;

	switch (id) {
	case 1: d = &mmp3_device_twsi1; break;
	case 2: d = &mmp3_device_twsi2; break;
	case 3: d = &mmp3_device_twsi3; break;
	case 4: d = &mmp3_device_twsi4; break;
	case 5: d = &mmp3_device_twsi5; break;
	case 6: d = &mmp3_device_twsi6; break;
	default:
		return -EINVAL;
	}

	ret = i2c_register_board_info(id - 1, info, size);
	if (ret)
		return ret;

	return pxa_register_device(d, data, sizeof(*data));
}

static inline int mmp3_add_cam(int id, struct mv_cam_pdata *cam)
{
	struct pxa_device_desc *d = NULL;

	switch (id) {
	case 0: d = &mmp3_device_camera0; break;
	case 1: d = &mmp3_device_camera1; break;
	default:
		return -EINVAL;
	}

       return pxa_register_device(d, cam, sizeof(*cam));
}

static inline int mmp3_add_nand(struct pxa3xx_nand_platform_data *info)
{
	return pxa_register_device(&mmp3_device_nand, info, sizeof(*info));
}

static inline int mmp3_add_sdh(int id, struct sdhci_pxa_platdata *data)
{
	struct pxa_device_desc *d = NULL;

	switch (id) {
	case 0: d = &mmp3_device_sdh0; break;
	case 1: d = &mmp3_device_sdh1; break;
	case 2: d = &mmp3_device_sdh2; break;
	case 3: d = &mmp3_device_sdh3; break;
	default:
		return -EINVAL;
	}

	return pxa_register_device(d, data, sizeof(*data));
}

static inline int mmp3_add_pwm(int id)
{
	struct pxa_device_desc *d = NULL;

	switch (id) {
	case 1:
		d = &mmp3_device_pwm1;
		break;
	case 2:
		d = &mmp3_device_pwm2;
		break;
	case 3:
		d = &mmp3_device_pwm3;
		break;
	case 4:
		d = &mmp3_device_pwm4;
		break;
	default:
		return -EINVAL;
	}

	return pxa_register_device(d, NULL, 0);
}

static inline int mmp3_add_fb(struct pxa168fb_mach_info *mi)
{
	return pxa_register_device(&mmp3_device_fb, mi, sizeof(*mi));
}

static inline int mmp3_add_fb_ovly(struct pxa168fb_mach_info *mi)
{
	return pxa_register_device(&mmp3_device_fb_ovly, mi, sizeof(*mi));
}

static inline void mmp3_add_audiosram(void)
{
	int ret;

	ret = platform_device_register(&mmp3_device_audiosram);

	if (ret)
		dev_err(&mmp3_device_audiosram.dev,
			"unable to register device: %d\n", ret);
}

static inline int mmp3_add_sspa(int id)
{
	struct pxa_device_desc *d = NULL;

	switch (id) {
	case 1:
		d = &mmp3_device_sspa1;
		break;
	case 2:
		d = &mmp3_device_sspa2;
		break;
	default:
		return -EINVAL;
	}

	return pxa_register_device(d, NULL, 0);
}

static inline void mmp3_add_rtc(void)
{
	int ret;
	ret = platform_device_register(&mmp3_device_rtc);
	if (ret)
		dev_err(&mmp3_device_rtc.dev,
				"unable to register device: %d\n", ret);
}

extern void mmp3_clear_keypad_wakeup(void);
static inline int mmp3_add_keypad(struct pxa27x_keypad_platform_data *data)
{
	data->clear_wakeup_event = mmp3_clear_keypad_wakeup;
	return pxa_register_device(&mmp3_device_keypad, data, sizeof(*data));
}

#endif /* __ASM_MACH_MMP2_H */
