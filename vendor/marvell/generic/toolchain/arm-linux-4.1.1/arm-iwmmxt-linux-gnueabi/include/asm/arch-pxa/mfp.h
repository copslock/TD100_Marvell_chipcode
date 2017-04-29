#ifndef _MHN_MFP_H
#define _MHN_MFP_H

typedef unsigned int mfp_pin_t;

struct mhn_pin_config {
	mfp_pin_t    mfp_pin;
	unsigned int reserved:16;
	unsigned int af_sel:3;
	unsigned int edge_rise_en:1;
	unsigned int edge_fall_en:1;
	unsigned int edge_clear:1;
	unsigned int sleep_oe_n:1;
	unsigned int sleep_data:1;
	unsigned int sleep_sel:1;
	unsigned int drive:3;
	unsigned int pulldown_en:1;
	unsigned int pullup_en:1;
	unsigned int pull_sel:1;
};

/*
 Table that determines the low power modes outputs, with actual settings
 used in parentheses for don't-care values.  Except for the float output,
 the configured driven and pulled levels match.  So if there is a need
 for non-LPM pulled output, the same configuration could probably be
 used.

 Output value    sleep_oe_n  sleep_data  pullup_en   pulldown_en pull_sel
                  (bit 7)     (bit 8)     (bit 14d)   (bit 13d)

 Drive 0             0          0           0           X (1)      0
 Drive 1             0          1           X (1)       0	   0
 Pull hi (1)         1          X(1)        1           0	   0
 Pull lo (0)         1          X(0)        0           1	   0
 Z (float)           1          X(0)        0           0	   0
*/
#define MFP_LPM_DRIVE_LOW	0x8
#define MFP_LPM_DRIVE_HIGH    	0x6
#define MFP_LPM_PULL_HIGH     	0x7 
#define MFP_LPM_PULL_LOW      	0x9 
#define MFP_LPM_FLOAT         	0x1
#define MFP_LPM_PULL_NEITHER	0x0

/*
 * The pullup and pulldown state of the MFP pin is by default determined by
 * selected alternate function. In case some buggy devices need to override
 * this default behavior, mhn_mfp_set_pull() can be invoked with one of the
 * following definition as the parameter.
 *
 * Definition       pull_sel  pullup_en  pulldown_en
 * MFP_PULL_HIGH        1         1        0
 * MFP_PULL_LOW         1         0        1
 * MFP_PULL_BOTH        1         1        1
 * MFP_PULL_NONE        1         0        0
 * MFP_PULL_DEFAULT     0         X        X
 *
 * NOTE: the function mhn_mfp_set_pull() will modify the bits of PULLUP_EN
 * and PULLDOWN_EN, which will cause potential conflicts with the low power
 * mode setting. Device drivers should take care of such conflicts, restore
 * the low power mode setting before entering low power mode if possible.
 */
#define MFP_PULL_BOTH		(0x7u)
#define MFP_PULL_HIGH		(0x6u)
#define MFP_PULL_LOW		(0x5u)
#define MFP_PULL_NONE		(0x4u)
#define MFP_PULL_DEFAULT	(0x0u)

#define MFP_AF0			(0)
#define MFP_AF1			(1)
#define MFP_AF2			(2)
#define MFP_AF3			(3)
#define MFP_AF4			(4)
#define MFP_AF5			(5)
#define MFP_AF6			(6)
#define MFP_AF7			(7)
#define MFP_DS01X		(0)
#define MFP_DS02X		(1)
#define MFP_DS03X		(2)
#define MFP_DS04X		(3)
#define MFP_DS06X		(4)
#define MFP_DS08X		(5)
#define MFP_DS10X		(6)
#define MFP_DS12X		(7)

#define MFP_EDGE_BOTH		0x3
#define MFP_EDGE_RISE		0x2
#define MFP_EDGE_FALL		0x1
#define MFP_EDGE_NONE		0x0

#define MFP_AF_MASK		0x0007
#define MFP_DRV_MASK		0x1c00
#define MFP_RDH_MASK		0x0200
#define MFP_LPM_MASK		0xe180
#define MFP_PULL_MASK		0xe000
#define MFP_EDGE_MASK		0x0070

#define MHN_MFP_CFG(desc, pin, af, drv, rdh, lpm, edge) \
{							\
	.mfp_pin = pin,					\
	.af_sel	 = af,					\
	.reserved	= 0,				\
	.drive		= drv,				\
	.sleep_sel	= rdh,				\
	.sleep_oe_n	= ((lpm) & 0x1),		\
	.sleep_data	= (((lpm) & 0x2)  >>1),		\
	.pullup_en	= (((lpm) & 0x4)  >>2),		\
	.pulldown_en	= (((lpm) & 0x8)  >>3),		\
	.pull_sel	= (((lpm) & 0x10) >>4),		\
	.edge_clear	= (!(edge)),			\
	.edge_fall_en	= ((edge) & 0x1),		\
	.edge_rise_en	= (((edge) & 0x2) >>1),		\
}

#define MFP_OFFSET(pin)	(((pin) >> 16) & 0xffffU)
#define MFP_REG(pin)	__REG(0x40E10000 + MFP_OFFSET(pin))

#define MFPR_ALT_OFFSET		0
#define MFPR_ERE_OFFSET		4
#define MFPR_EFE_OFFSET		5
#define MFPR_EC_OFFSET		6
#define MFPR_SON_OFFSET		7
#define MFPR_SD_OFFSET		8
#define MFPR_SS_OFFSET		9
#define MFPR_DRV_OFFSET		10
#define MFPR_PD_OFFSET		13	
#define MFPR_PU_OFFSET		14	
#define MFPR_PS_OFFSET		15	

#define PIN2REG(pin_config)					\
		(pin_config->af_sel << MFPR_ALT_OFFSET) | 	\
                (pin_config->edge_rise_en << MFPR_ERE_OFFSET ) |\
                (pin_config->edge_fall_en << MFPR_EFE_OFFSET ) |\
                (pin_config->edge_clear << MFPR_EC_OFFSET ) |	\
                (pin_config->sleep_oe_n << MFPR_SON_OFFSET ) |	\
                (pin_config->sleep_data << MFPR_SD_OFFSET ) |	\
                (pin_config->sleep_sel << MFPR_SS_OFFSET ) |	\
                (pin_config->drive << MFPR_DRV_OFFSET ) |	\
                (pin_config->pulldown_en << MFPR_PD_OFFSET ) |	\
                (pin_config->pullup_en << MFPR_PU_OFFSET ) |	\
                (pin_config->pull_sel << MFPR_PS_OFFSET );

#if  defined(CONFIG_CPU_MONAHANS_P) || defined(CONFIG_CPU_MONAHANS_PL)
#define	MFP_PIN_GPIO0		((0x0124 << 16) | (0))
#define	MFP_PIN_GPIO1		((0x0128 << 16) | (1))
#define	MFP_PIN_GPIO2		((0x012C << 16) | (2))
#define	MFP_PIN_GPIO3		((0x0130 << 16) | (3))
#define	MFP_PIN_GPIO4		((0x0134 << 16) | (4))
#define	MFP_PIN_nXCVREN		((0x0138 << 16) | (0xff))
#define	MFP_PIN_ND_CLE		((0x0204 << 16) | (0xff))
#define	MFP_PIN_DF_nADV1_ALE	((0x0208 << 16) | (0xff))
#define	MFP_PIN_DF_SCLK_S	((0x020C << 16) | (0xff))
#define	MFP_PIN_DF_SCLK_E	((0x0210 << 16) | (0xff))
#define	MFP_PIN_nBE0		((0x0214 << 16) | (0xff))
#define	MFP_PIN_nBE1		((0x0218 << 16) | (0xff))
#define	MFP_PIN_DF_nADV2_ALE	((0x021C << 16) | (0xff))
#define	MFP_PIN_DF_INT_RnB	((0x0220 << 16) | (0xff))
#define	MFP_PIN_DF_nCS0		((0x0224 << 16) | (0xff))
#define	MFP_PIN_DF_nCS1		((0x0228 << 16) | (0xff))
#define	MFP_PIN_DF_nWE		((0x022C << 16) | (0xff))
#define	MFP_PIN_DF_nRE_nOE	((0x0230 << 16) | (0xff))
#define	MFP_PIN_nLUA		((0x0234 << 16) | (0xff))
#define	MFP_PIN_nLLA		((0x0238 << 16) | (0xff))
#define	MFP_PIN_DF_ADDR0	((0x023C << 16) | (0xff))
#define	MFP_PIN_DF_ADDR1	((0x0240 << 16) | (0xff))
#define	MFP_PIN_DF_ADDR2	((0x0244 << 16) | (0xff))
#define	MFP_PIN_DF_ADDR3	((0x0248 << 16) | (0xff))
#define	MFP_PIN_DF_IO0		((0x024C << 16) | (0xff))
#define	MFP_PIN_DF_IO8		((0x0250 << 16) | (0xff))
#define	MFP_PIN_DF_IO1		((0x0254 << 16) | (0xff))
#define	MFP_PIN_DF_IO9		((0x0258 << 16) | (0xff))
#define	MFP_PIN_DF_IO2		((0x025C << 16) | (0xff))
#define	MFP_PIN_DF_IO10		((0x0260 << 16) | (0xff))
#define	MFP_PIN_DF_IO3		((0x0264 << 16) | (0xff))
#define	MFP_PIN_DF_IO11		((0x0268 << 16) | (0xff))
#define	MFP_PIN_DF_IO4		((0x026C << 16) | (0xff))
#define	MFP_PIN_DF_IO12		((0x0270 << 16) | (0xff))
#define	MFP_PIN_DF_IO5		((0x0274 << 16) | (0xff))
#define	MFP_PIN_DF_IO13		((0x0278 << 16) | (0xff))
#define	MFP_PIN_DF_IO6		((0x027C << 16) | (0xff))
#define	MFP_PIN_DF_IO14		((0x0280 << 16) | (0xff))
#define	MFP_PIN_DF_IO7		((0x0284 << 16) | (0xff))
#define	MFP_PIN_DF_IO15		((0x0288 << 16) | (0xff))
#define	MFP_PIN_GPIO5		((0x028C << 16) | (5))
#define	MFP_PIN_GPIO6		((0x0290 << 16) | (6))
#define	MFP_PIN_GPIO7		((0x0294 << 16) | (7))
#define	MFP_PIN_GPIO8		((0x0298 << 16) | (8))
#define	MFP_PIN_GPIO9		((0x029C << 16) | (9))
#define	MFP_PIN_GPIO11		((0x02A0 << 16) | (11))
#define	MFP_PIN_GPIO12		((0x02A4 << 16) | (12))
#define	MFP_PIN_GPIO13		((0x02A8 << 16) | (13))
#define	MFP_PIN_GPIO14		((0x02AC << 16) | (14))
#define	MFP_PIN_GPIO15		((0x02B0 << 16) | (15))
#define	MFP_PIN_GPIO16		((0x02B4 << 16) | (16))
#define	MFP_PIN_GPIO17		((0x02B8 << 16) | (17))
#define	MFP_PIN_GPIO18		((0x02BC << 16) | (18))
#define	MFP_PIN_GPIO19		((0x02C0 << 16) | (19))
#define	MFP_PIN_GPIO20		((0x02C4 << 16) | (20))
#define	MFP_PIN_GPIO21		((0x02C8 << 16) | (21))
#define	MFP_PIN_GPIO22		((0x02CC << 16) | (22))
#define	MFP_PIN_GPIO23		((0x02D0 << 16) | (23))
#define	MFP_PIN_GPIO24		((0x02D4 << 16) | (24))
#define	MFP_PIN_GPIO25		((0x02D8 << 16) | (25))
#define	MFP_PIN_GPIO26		((0x02DC << 16) | (26))
#define	MFP_PIN_GPIO27		((0x0400 << 16) | (27))
#define	MFP_PIN_GPIO28		((0x0404 << 16) | (28))
#define	MFP_PIN_GPIO29		((0x0408 << 16) | (29))
#define	MFP_PIN_GPIO30		((0x040C << 16) | (30))
#define	MFP_PIN_GPIO31		((0x0410 << 16) | (31))
#define	MFP_PIN_GPIO32		((0x0414 << 16) | (32))
#define	MFP_PIN_GPIO33		((0x0418 << 16) | (33))
#define	MFP_PIN_GPIO34		((0x041C << 16) | (34))
#define	MFP_PIN_GPIO35		((0x0420 << 16) | (35))
#define	MFP_PIN_GPIO36		((0x0424 << 16) | (36))
#define	MFP_PIN_GPIO37		((0x0428 << 16) | (37))
#define	MFP_PIN_GPIO38		((0x042C << 16) | (38))
#define	MFP_PIN_GPIO39		((0x0430 << 16) | (39))
#define	MFP_PIN_GPIO40		((0x0434 << 16) | (40))
#define	MFP_PIN_GPIO41		((0x0438 << 16) | (41))
#define	MFP_PIN_GPIO42		((0x043C << 16) | (42))
#define	MFP_PIN_GPIO43		((0x0440 << 16) | (43))
#define	MFP_PIN_GPIO44		((0x0444 << 16) | (44))
#define	MFP_PIN_GPIO45		((0x0448 << 16) | (45))
#define	MFP_PIN_GPIO46		((0x044C << 16) | (46))
#define	MFP_PIN_GPIO47		((0x0450 << 16) | (47))
#define	MFP_PIN_GPIO48		((0x0454 << 16) | (48))
#define	MFP_PIN_GPIO10		((0x0458 << 16) | (10))
#define	MFP_PIN_GPIO49		((0x045C << 16) | (49))
#define	MFP_PIN_GPIO50		((0x0460 << 16) | (50))
#define	MFP_PIN_GPIO51		((0x0464 << 16) | (51))
#define	MFP_PIN_GPIO52		((0x0468 << 16) | (52))
#define	MFP_PIN_GPIO53		((0x046C << 16) | (53))
#define	MFP_PIN_GPIO54		((0x0470 << 16) | (54))
#define	MFP_PIN_GPIO55		((0x0474 << 16) | (55))
#define	MFP_PIN_GPIO56		((0x0478 << 16) | (56))
#define	MFP_PIN_GPIO57		((0x047C << 16) | (57))
#define	MFP_PIN_GPIO58		((0x0480 << 16) | (58))
#define	MFP_PIN_GPIO59		((0x0484 << 16) | (59))
#define	MFP_PIN_GPIO60		((0x0488 << 16) | (60))
#define	MFP_PIN_GPIO61		((0x048C << 16) | (61))
#define	MFP_PIN_GPIO62		((0x0490 << 16) | (62))
#define	MFP_PIN_GPIO6_2		((0x0494 << 16) | (6))
#define	MFP_PIN_GPIO7_2		((0x0498 << 16) | (7))
#define	MFP_PIN_GPIO8_2		((0x049C << 16) | (8))
#define	MFP_PIN_GPIO9_2		((0x04A0 << 16) | (9))
#define	MFP_PIN_GPIO10_2	((0x04A4 << 16) | (10))
#define	MFP_PIN_GPIO11_2	((0x04A8 << 16) | (11))
#define	MFP_PIN_GPIO12_2	((0x04AC << 16) | (12))
#define	MFP_PIN_GPIO13_2	((0x04B0 << 16) | (13))
#define	MFP_PIN_GPIO63		((0x04B4 << 16) | (63))
#define	MFP_PIN_GPIO64		((0x04B8 << 16) | (64))
#define	MFP_PIN_GPIO65		((0x04BC << 16) | (65))
#define	MFP_PIN_GPIO66		((0x04C0 << 16) | (66))
#define	MFP_PIN_GPIO67		((0x04C4 << 16) | (67))
#define	MFP_PIN_GPIO68		((0x04C8 << 16) | (68))
#define	MFP_PIN_GPIO69		((0x04CC << 16) | (69))
#define	MFP_PIN_GPIO70		((0x04D0 << 16) | (70))
#define	MFP_PIN_GPIO71		((0x04D4 << 16) | (71))
#define	MFP_PIN_GPIO72		((0x04D8 << 16) | (72))
#define	MFP_PIN_GPIO73		((0x04DC << 16) | (73))
#define	MFP_PIN_GPIO14_2	((0x04E0 << 16) | (14))
#define	MFP_PIN_GPIO15_2	((0x04E4 << 16) | (15))
#define	MFP_PIN_GPIO16_2	((0x04E8 << 16) | (16))
#define	MFP_PIN_GPIO17_2	((0x04EC << 16) | (17))
#define	MFP_PIN_GPIO74		((0x04F0 << 16) | (74))
#define	MFP_PIN_GPIO75		((0x04F4 << 16) | (75))
#define	MFP_PIN_GPIO76		((0x04F8 << 16) | (76))
#define	MFP_PIN_GPIO77		((0x04FC << 16) | (77))
#define	MFP_PIN_GPIO78		((0x0500 << 16) | (78))
#define	MFP_PIN_GPIO79		((0x0504 << 16) | (79))
#define	MFP_PIN_GPIO80		((0x0508 << 16) | (80))
#define	MFP_PIN_GPIO81		((0x050C << 16) | (81))
#define	MFP_PIN_GPIO82		((0x0510 << 16) | (82))
#define	MFP_PIN_GPIO83		((0x0514 << 16) | (83))
#define	MFP_PIN_GPIO84		((0x0518 << 16) | (84))
#define	MFP_PIN_GPIO85		((0x051C << 16) | (85))
#define	MFP_PIN_GPIO86		((0x0520 << 16) | (86))
#define	MFP_PIN_GPIO87		((0x0524 << 16) | (87))
#define	MFP_PIN_GPIO88		((0x0528 << 16) | (88))
#define	MFP_PIN_GPIO89		((0x052C << 16) | (89))
#define	MFP_PIN_GPIO90		((0x0530 << 16) | (90))
#define	MFP_PIN_GPIO91		((0x0534 << 16) | (91))
#define	MFP_PIN_GPIO92		((0x0538 << 16) | (92))
#define	MFP_PIN_GPIO93		((0x053C << 16) | (93))
#define	MFP_PIN_GPIO94		((0x0540 << 16) | (94))
#define	MFP_PIN_GPIO95		((0x0544 << 16) | (95))
#define	MFP_PIN_GPIO96		((0x0548 << 16) | (96))
#define	MFP_PIN_GPIO97		((0x054C << 16) | (97))
#define	MFP_PIN_GPIO98		((0x0550 << 16) | (98))
#define	MFP_PIN_GPIO99		((0x0600 << 16) | (99))
#define	MFP_PIN_GPIO100		((0x0604 << 16) | (100))
#define	MFP_PIN_GPIO101		((0x0608 << 16) | (101))
#define	MFP_PIN_GPIO102		((0x060C << 16) | (102))
#define	MFP_PIN_GPIO103		((0x0610 << 16) | (103))
#define	MFP_PIN_GPIO104		((0x0614 << 16) | (104))
#define	MFP_PIN_GPIO105		((0x0618 << 16) | (105))
#define	MFP_PIN_GPIO106		((0x061C << 16) | (106))
#define	MFP_PIN_GPIO107		((0x0620 << 16) | (107))
#define	MFP_PIN_GPIO108		((0x0624 << 16) | (108))
#define	MFP_PIN_GPIO109		((0x0628 << 16) | (109))
#define	MFP_PIN_GPIO110		((0x062C << 16) | (110))
#define	MFP_PIN_GPIO111		((0x0630 << 16) | (111))
#define	MFP_PIN_GPIO112		((0x0634 << 16) | (112))
#define	MFP_PIN_GPIO113		((0x0638 << 16) | (113))
#define	MFP_PIN_GPIO114		((0x063C << 16) | (114))
#define	MFP_PIN_GPIO115		((0x0640 << 16) | (115))
#define	MFP_PIN_GPIO116		((0x0644 << 16) | (116))
#define	MFP_PIN_GPIO117		((0x0648 << 16) | (117))
#define	MFP_PIN_GPIO118		((0x064C << 16) | (118))
#define	MFP_PIN_GPIO119		((0x0650 << 16) | (119))
#define	MFP_PIN_GPIO120		((0x0654 << 16) | (120))
#define	MFP_PIN_GPIO121		((0x0658 << 16) | (121))
#define	MFP_PIN_GPIO122		((0x065C << 16) | (122))
#define	MFP_PIN_GPIO123		((0x0660 << 16) | (123))
#define	MFP_PIN_GPIO124		((0x0664 << 16) | (124))
#define	MFP_PIN_GPIO125		((0x0668 << 16) | (125))
#define	MFP_PIN_GPIO126		((0x066C << 16) | (126))
#define	MFP_PIN_GPIO127		((0x0670 << 16) | (127))
#define	MFP_PIN_GPIO0_2		((0x0674 << 16) | (0))
#define	MFP_PIN_GPIO1_2		((0x0678 << 16) | (1))
#define	MFP_PIN_GPIO2_2		((0x067C << 16) | (2))
#define	MFP_PIN_GPIO3_2		((0x0680 << 16) | (3))
#define	MFP_PIN_GPIO4_2		((0x0684 << 16) | (4))
#define	MFP_PIN_GPIO5_2		((0x0688 << 16) | (5))

#define MHN_MIN_MFP_OFFSET	(MFP_OFFSET(MFP_PIN_GPIO0))
#define MHN_MAX_MFP_OFFSET	(MFP_OFFSET(MFP_PIN_GPIO5_2))

#elif defined(CONFIG_CPU_MONAHANS_L)
#define	MFP_PIN_GPIO0		((0x00B4 << 16) | (0))
#define	MFP_PIN_GPIO1		((0x00B8 << 16) | (1))
#define	MFP_PIN_GPIO2		((0x00BC << 16) | (2))
#define	MFP_PIN_GPIO3		((0x027C << 16) | (3))
#define	MFP_PIN_GPIO4		((0x0280 << 16) | (4))
#define MFP_PIN_DF_SCLK_E_MFPR	((0x0250 << 16) | (0xff))
#define	MFP_PIN_nBE0		((0x0204 << 16) | (0xff))
#define	MFP_PIN_nBE1		((0x0208 << 16) | (0xff))
#define	MFP_PIN_DF_ALE_nWE	((0x020C << 16) | (0xff))
#define	MFP_PIN_DF_INT_RnB	((0x00C8 << 16) | (0xff))
#define	MFP_PIN_DF_nCS0		((0x0248 << 16) | (0xff))
#define	MFP_PIN_DF_nCS1		((0x0278 << 16) | (0xff))
#define	MFP_PIN_DF_nWE		((0x00CC << 16) | (0xff))
#define	MFP_PIN_DF_nRE		((0x0200 << 16) | (0xff))
#define	MFP_PIN_nLUA		((0x0244 << 16) | (0xff))
#define	MFP_PIN_nLLA		((0x0254 << 16) | (0xff))
#define	MFP_PIN_DF_ADDR0	((0x0210 << 16) | (0xff))
#define	MFP_PIN_DF_ADDR1	((0x0214 << 16) | (0xff))
#define	MFP_PIN_DF_ADDR2	((0x0218 << 16) | (0xff))
#define	MFP_PIN_DF_ADDR3	((0x021C << 16) | (0xff))
#define MFP_PIN_CLE_nOE		((0x0240 << 16) | (0xff))
#define	MFP_PIN_DF_IO0		((0x0220 << 16) | (0xff))
#define	MFP_PIN_DF_IO8		((0x0224 << 16) | (0xff))
#define	MFP_PIN_DF_IO1		((0x0228 << 16) | (0xff))
#define	MFP_PIN_DF_IO9		((0x022C << 16) | (0xff))
#define	MFP_PIN_DF_IO2		((0x0230 << 16) | (0xff))
#define	MFP_PIN_DF_IO10		((0x0234 << 16) | (0xff))
#define	MFP_PIN_DF_IO3		((0x0238 << 16) | (0xff))
#define	MFP_PIN_DF_IO11		((0x023C << 16) | (0xff))
#define	MFP_PIN_DF_IO4		((0x0258 << 16) | (0xff))
#define	MFP_PIN_DF_IO12		((0x025C << 16) | (0xff))
#define	MFP_PIN_DF_IO5		((0x0260 << 16) | (0xff))
#define	MFP_PIN_DF_IO13		((0x0264 << 16) | (0xff))
#define	MFP_PIN_DF_IO6		((0x0268 << 16) | (0xff))
#define	MFP_PIN_DF_IO14		((0x026C << 16) | (0xff))
#define	MFP_PIN_DF_IO7		((0x0270 << 16) | (0xff))
#define	MFP_PIN_DF_IO15		((0x0274 << 16) | (0xff))
#define	MFP_PIN_GPIO5		((0x0284 << 16) | (5))
#define	MFP_PIN_GPIO6		((0x0288 << 16) | (6))
#define	MFP_PIN_GPIO7		((0x028C << 16) | (7))
#define	MFP_PIN_GPIO8		((0x0290 << 16) | (8))
#define	MFP_PIN_GPIO9		((0x0294 << 16) | (9))
#define	MFP_PIN_GPIO10		((0x0298 << 16) | (9))
#define	MFP_PIN_GPIO11		((0x029C << 16) | (11))
#define	MFP_PIN_GPIO12		((0x02A0 << 16) | (12))
#define	MFP_PIN_GPIO13		((0x02A4 << 16) | (13))
#define	MFP_PIN_GPIO14		((0x02A8 << 16) | (14))
#define	MFP_PIN_GPIO15		((0x02AC << 16) | (15))
#define	MFP_PIN_GPIO16		((0x02B0 << 16) | (16))
#define	MFP_PIN_GPIO17		((0x02B4 << 16) | (17))
#define	MFP_PIN_GPIO18		((0x02B8 << 16) | (18))
#define	MFP_PIN_GPIO19		((0x02BC << 16) | (19))
#define	MFP_PIN_GPIO20		((0x02C0 << 16) | (20))
#define	MFP_PIN_GPIO21		((0x02C4 << 16) | (21))
#define	MFP_PIN_GPIO22		((0x02C8 << 16) | (22))
#define	MFP_PIN_GPIO23		((0x02CC << 16) | (23))
#define	MFP_PIN_GPIO24		((0x02D0 << 16) | (24))
#define	MFP_PIN_GPIO25		((0x02D4 << 16) | (25))
#define	MFP_PIN_GPIO26		((0x02D8 << 16) | (26))
#define	MFP_PIN_GPIO27		((0x0400 << 16) | (27))
#define	MFP_PIN_GPIO28		((0x0404 << 16) | (28))
#define	MFP_PIN_GPIO29		((0x0408 << 16) | (29))
#define	MFP_PIN_GPIO30		((0x040C << 16) | (30))
#define	MFP_PIN_GPIO31		((0x0410 << 16) | (31))
#define	MFP_PIN_GPIO32		((0x0414 << 16) | (32))
#define	MFP_PIN_GPIO33		((0x0418 << 16) | (33))
#define	MFP_PIN_GPIO34		((0x041C << 16) | (34))
#define	MFP_PIN_GPIO35		((0x0420 << 16) | (35))
#define	MFP_PIN_GPIO36		((0x0424 << 16) | (36))
#define	MFP_PIN_GPIO37		((0x0428 << 16) | (37))
#define	MFP_PIN_GPIO38		((0x042C << 16) | (38))
#define	MFP_PIN_GPIO39		((0x0430 << 16) | (39))
#define	MFP_PIN_GPIO40		((0x0434 << 16) | (40))
#define	MFP_PIN_GPIO41		((0x0438 << 16) | (41))
#define	MFP_PIN_GPIO42		((0x043C << 16) | (42))
#define	MFP_PIN_GPIO43		((0x0440 << 16) | (43))
#define	MFP_PIN_GPIO44		((0x0444 << 16) | (44))
#define	MFP_PIN_GPIO45		((0x0448 << 16) | (45))
#define	MFP_PIN_GPIO46		((0x044C << 16) | (46))
#define	MFP_PIN_GPIO47		((0x0450 << 16) | (47))
#define	MFP_PIN_GPIO48		((0x0454 << 16) | (48))
#define	MFP_PIN_GPIO49		((0x0458 << 16) | (49))
#define	MFP_PIN_GPIO50		((0x045C << 16) | (50))
#define	MFP_PIN_GPIO51		((0x0460 << 16) | (51))
#define	MFP_PIN_GPIO52		((0x0464 << 16) | (52))
#define	MFP_PIN_GPIO53		((0x0468 << 16) | (53))
#define	MFP_PIN_GPIO54		((0x046C << 16) | (54))
#define	MFP_PIN_GPIO55		((0x0470 << 16) | (55))
#define	MFP_PIN_GPIO56		((0x0474 << 16) | (56))
#define	MFP_PIN_GPIO57		((0x0478 << 16) | (57))
#define	MFP_PIN_GPIO58		((0x047C << 16) | (58))
#define	MFP_PIN_GPIO59		((0x0480 << 16) | (59))
#define	MFP_PIN_GPIO60		((0x0484 << 16) | (60))
#define	MFP_PIN_GPIO61		((0x0488 << 16) | (61))
#define	MFP_PIN_GPIO62		((0x048C << 16) | (62))
#define	MFP_PIN_GPIO63		((0x0490 << 16) | (63))
#define	MFP_PIN_GPIO64		((0x0494 << 16) | (64))
#define	MFP_PIN_GPIO65		((0x0498 << 16) | (65))
#define	MFP_PIN_GPIO66		((0x049C << 16) | (66))
#define	MFP_PIN_GPIO67		((0x04A0 << 16) | (67))
#define	MFP_PIN_GPIO68		((0x04A4 << 16) | (68))
#define	MFP_PIN_GPIO69		((0x04A8 << 16) | (69))
#define	MFP_PIN_GPIO70		((0x04AC << 16) | (70))
#define	MFP_PIN_GPIO71		((0x04B0 << 16) | (71))
#define	MFP_PIN_GPIO72		((0x04B4 << 16) | (72))
#define	MFP_PIN_GPIO73		((0x04B8 << 16) | (73))
#define	MFP_PIN_GPIO74		((0x04BC << 16) | (74))
#define	MFP_PIN_GPIO75		((0x04C0 << 16) | (75))
#define	MFP_PIN_GPIO76		((0x04C4 << 16) | (76))
#define	MFP_PIN_GPIO77		((0x04C8 << 16) | (77))
#define	MFP_PIN_GPIO78		((0x04CC << 16) | (78))
#define	MFP_PIN_GPIO79		((0x04D0 << 16) | (79))
#define	MFP_PIN_GPIO80		((0x04D4 << 16) | (80))
#define	MFP_PIN_GPIO81		((0x04D8 << 16) | (81))
#define	MFP_PIN_GPIO82		((0x04DC << 16) | (82))
#define	MFP_PIN_GPIO83		((0x04E0 << 16) | (83))
#define	MFP_PIN_GPIO84		((0x04E4 << 16) | (84))
#define	MFP_PIN_GPIO85		((0x04E8 << 16) | (85))
#define	MFP_PIN_GPIO86		((0x04EC << 16) | (86))
#define	MFP_PIN_GPIO87		((0x04F0 << 16) | (87))
#define	MFP_PIN_GPIO88		((0x04F4 << 16) | (88))
#define	MFP_PIN_GPIO89		((0x04F8 << 16) | (89))
#define	MFP_PIN_GPIO90		((0x04FC << 16) | (90))
#define	MFP_PIN_GPIO91		((0x0500 << 16) | (91))
#define	MFP_PIN_GPIO92		((0x0504 << 16) | (92))
#define	MFP_PIN_GPIO93		((0x0508 << 16) | (93))
#define	MFP_PIN_GPIO94		((0x050C << 16) | (94))
#define	MFP_PIN_GPIO95		((0x0510 << 16) | (95))
#define	MFP_PIN_GPIO96		((0x0514 << 16) | (96))
#define	MFP_PIN_GPIO97		((0x0518 << 16) | (97))
#define	MFP_PIN_GPIO98		((0x051C << 16) | (98))
#define	MFP_PIN_GPIO99		((0x0600 << 16) | (99))
#define	MFP_PIN_GPIO100		((0x0604 << 16) | (100))
#define	MFP_PIN_GPIO101		((0x0608 << 16) | (101))
#define	MFP_PIN_GPIO102		((0x060C << 16) | (102))
#define	MFP_PIN_GPIO103		((0x0610 << 16) | (103))
#define	MFP_PIN_GPIO104		((0x0614 << 16) | (104))
#define	MFP_PIN_GPIO105		((0x0618 << 16) | (105))
#define	MFP_PIN_GPIO106		((0x061C << 16) | (106))
#define	MFP_PIN_GPIO107		((0x0620 << 16) | (107))
#define	MFP_PIN_GPIO108		((0x0624 << 16) | (108))
#define	MFP_PIN_GPIO109		((0x0628 << 16) | (109))
#define	MFP_PIN_GPIO110		((0x062C << 16) | (110))
#define	MFP_PIN_GPIO111		((0x0630 << 16) | (111))
#define	MFP_PIN_GPIO112		((0x0634 << 16) | (112))
#define	MFP_PIN_GPIO113		((0x0638 << 16) | (113))
#define	MFP_PIN_GPIO114		((0x063C << 16) | (114))
#define	MFP_PIN_GPIO115		((0x0640 << 16) | (115))
#define	MFP_PIN_GPIO116		((0x0644 << 16) | (116))
#define	MFP_PIN_GPIO117		((0x0648 << 16) | (117))
#define	MFP_PIN_GPIO118		((0x064C << 16) | (118))
#define	MFP_PIN_GPIO119		((0x0650 << 16) | (119))
#define	MFP_PIN_GPIO120		((0x0654 << 16) | (120))
#define	MFP_PIN_GPIO121		((0x0658 << 16) | (121))
#define	MFP_PIN_GPIO122		((0x065C << 16) | (122))
#define	MFP_PIN_GPIO123		((0x0660 << 16) | (123))
#define	MFP_PIN_GPIO124		((0x0664 << 16) | (124))
#define	MFP_PIN_GPIO125		((0x0668 << 16) | (125))
#define	MFP_PIN_GPIO126		((0x066C << 16) | (126))
#define	MFP_PIN_GPIO127		((0x0670 << 16) | (127))
#define	MFP_PIN_GPIO0_2		((0x0674 << 16) | (0))
#define	MFP_PIN_GPIO1_2		((0x0678 << 16) | (1))
#define	MFP_PIN_GPIO2_2		((0x02DC << 16) | (2))
#define	MFP_PIN_GPIO3_2		((0x02E0 << 16) | (3))
#define	MFP_PIN_GPIO4_2		((0x02E4 << 16) | (4))
#define	MFP_PIN_GPIO5_2		((0x02E8 << 16) | (5))
#define	MFP_PIN_GPIO6_2		((0x02EC << 16) | (6))

#define MHN_MIN_MFP_OFFSET	(MFP_OFFSET(MFP_PIN_GPIO0))
#define MHN_MAX_MFP_OFFSET	(MFP_OFFSET(MFP_PIN_GPIO1_2))

#else
#error "please define GPIOs for your CPU type!"
#endif


extern int mhn_mfp_set_config (struct mhn_pin_config *pin_config);
extern int mhn_mfp_set_configs(struct mhn_pin_config *pin_configs, int n);

extern int mhn_mfp_set_afds(mfp_pin_t pin, int af, int ds);
extern int mhn_mfp_set_lpm (mfp_pin_t pin, int lpm);
extern int mhn_mfp_set_rdh (mfp_pin_t pin, int rdh);
extern int mhn_mfp_set_edge(mfp_pin_t pin, int edge);
extern int mhn_mfp_set_pull(mfp_pin_t pin, int pull);

#endif /* _MHN_MFP_H */
