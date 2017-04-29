#ifndef _ARAVA_H_
#define _ARAVA_H_
#include <asm/arch/i2c-pxa.h>

/* SYSMON */
#define ARAVA_CHIP_ID	0x00
#define ARAVA_EVENT_A 	0x01
#define ARAVA_EVENT_B 	0x02
#define ARAVA_EVENT_C 	0x03
#define ARAVA_STATUS  	0x04
#define ARAVA_IRQ_MASK_A 	0x05
#define ARAVA_IRQ_MASK_B 	0x06
#define ARAVA_IRQ_MASK_C 	0x07
#define ARAVA_SYSCTRL_A 	0x08
#define ARAVA_SYSCTRL_B	0x09
#define ARAVA_SYSCTRL_C 	0x80
#define ARAVA_FAULT_LOG	0x0A

/* REG */
#define ARAVA_LDO1011		0x10
#define ARAVA_LDO15		0x11
#define ARAVA_LDO1416		0x12
#define ARAVA_LDO1819		0x13
#define ARAVA_LDO17_SIMCP0		0x14
#define ARAVA_BUCK2DVC1		0x15
#define ARAVA_BUCK2DVC2		0x16
#define ARAVA_REGCTRL1		0x17
#define ARAVA_REGCTRL2		0x18
#define ARAVA_USBPUMP		0x19
#define ARAVA_APPSLEEP_CTRL	0x1A
#define ARAVA_STARTUP_CTRL		0x1B

/* LED - ignored now. Skip */
#define ARAVA_LED1_CTRL		0x20
#define ARAVA_LED2_CTRL		0x21
#define ARAVA_LED3_CTRL		0x23
#define ARAVA_LED4_CTRL		0x24
#define ARAVA_LEDPC_CTRL		0x25
#define ARAVA_WLED_CTRL		0x26

/* MISC */
#define ARAVA_MISCA 0x26
#define ARAVA_MISCB 0x27

/* Charge */
#define ARAVA_CHARGE_CTRL		0x28
#define ARAVA_CCTR_CTRL		0x29
#define ARAVA_TCTR_CTRL		0x2A
#define ARAVA_CHARGE_PULSE		0x2B

#define ARAVA_ADDRESS 0x49


#define VBASE	1100
#define VSTEP	50
#define VMAX	2650

#define VBUCK2BASE	850
#define VBUCK2STEP	25
#define VBUCK2MAX	1625



extern int arava_write(u8 reg, u8 val);
extern int arava_read(u8 reg, u8 *pval);

extern int arava_get_vcc_core(void);
extern int arava_get_vcc_sram(void);
extern int arava_get_vcc_io(void);
extern int arava_set_vcc_io(unsigned int);
extern int arava_get_vcc_msl(void);
extern int arava_set_vcc_msl(unsigned int);
extern int arava_get_vcc_lcd(void);
extern int arava_set_vcc_lcd(unsigned int);
extern int arava_get_vcc_usb(void);
extern int arava_set_vcc_usb(unsigned int);
extern int arava_get_vcc_card0(void);
extern int arava_set_vcc_card0(unsigned int);
extern int arava_get_vcc_card1(void);
extern int arava_set_vcc_card1(unsigned int);
extern int arava_get_vcc_mem(void);
extern int arava_set_vcc_mem(unsigned int);
extern int arava_get_vcc_df(void);
extern int arava_set_vcc_df(unsigned int);
extern int arava_get_vcc_ci(void);
extern int arava_set_vcc_ci(unsigned int);
extern int arava_get_vcc_tsi(void);
extern int arava_set_vcc_tsi(unsigned int);
extern int arava_write(u8 reg, u8 val);
extern int arava_read(u8 reg, u8 * pval);
extern int arava_set_vcc_sram(unsigned int mv);
extern int arava_set_vcc_core(unsigned int mv);
extern int arava_get_sramvcc(u32 *pmv);
extern int arava_get_corevcc(u32 *pmv);

#endif












