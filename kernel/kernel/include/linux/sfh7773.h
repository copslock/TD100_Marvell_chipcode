#ifndef __BH1770GLC_H__
#define __BH1770GLC_H__

struct bh1770glc_platform_data {
/* IR-Led configuration for proximity sensing */
#define BH1770GLC_LED1     0x00
#define BH1770GLC_LED12            0x01
#define BH1770GLC_LED13            0x02
#define BH1770GLC_LED123    0x03

       __u8 leds;
/* led_max_curr is a safetylimit for IR leds */
#define BH1770GLC_LED_5mA   0
#define BH1770GLC_LED_10mA  1
#define BH1770GLC_LED_20mA  2
#define BH1770GLC_LED_50mA  3
#define BH1770GLC_LED_100mA 4
#define BH1770GLC_LED_150mA 5
#define BH1770GLC_LED_200mA 6
       __u8 led_max_curr;
       __u8 led_def_curr[3];

       int (*setup_resources)(void);
       int (*release_resources)(void);
};

/* Device name: /dev/bh1770glc_ps */
struct bh1770glc_ps {
       __u8 led1;
       __u8 led2;
       __u8 led3;
} __attribute__((packed));

/* Device name: /dev/bh1770glc_als */
struct bh1770glc_als {
       __u16 lux;
} __attribute__((packed));

#endif
