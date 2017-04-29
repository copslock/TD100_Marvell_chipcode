/*
 * linux/arch/arm/mach-pxa/include/mach/pxa950fb.h
 *
 *  Copyright (C) 2009 Marvell International Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_MACH_PXA950FB_H
#define __ASM_MACH_PXA950FB_H
/* ---------------------------------------------- */
/*              Header Files                      */
/* ---------------------------------------------- */
#include <linux/fb.h>

#define PXA950FB_FB_NUM           2
/* ---------------------------------------------- */
/*              IOCTL Definition                  */
/* ---------------------------------------------- */
#define FB_IOC_MAGIC                        'm'
#define FB_IOCTL_CONFIG_CURSOR              _IO(FB_IOC_MAGIC, 0)
#define FB_IOCTL_DUMP_REGS                  _IO(FB_IOC_MAGIC, 1)
#define FB_IOCTL_CLEAR_IRQ                  _IO(FB_IOC_MAGIC, 2)

/*
 * There are many video mode supported.
 */
#define FB_IOCTL_SET_VIDEO_MODE             _IO(FB_IOC_MAGIC, 3)
#define FB_IOCTL_GET_VIDEO_MODE             _IO(FB_IOC_MAGIC, 4)
/* Request a new video buffer from driver. User program needs to free
 * this memory.
 */
#define FB_IOCTL_CREATE_VID_BUFFER          _IO(FB_IOC_MAGIC, 5)

/* Configure viewport in driver. */
#define FB_IOCTL_SET_VIEWPORT_INFO          _IO(FB_IOC_MAGIC, 6)
#define FB_IOCTL_GET_VIEWPORT_INFO          _IO(FB_IOC_MAGIC, 7)

/* Flip the video buffer from user mode. Vide buffer can be separated into:
 * a. Current-used buffer - user program put any data into it. It will be
 *    displayed immediately.
 * b. Requested from driver but not current-used - user programe can put any
 *    data into it. It will be displayed after calling
 *    FB_IOCTL_FLIP_VID_BUFFER.
 *    User program should free this memory when they don't use it any more.
 * c. User program alloated - user program can allocated a contiguos DMA
 *    buffer to store its video data. And flip it to driver. Notices that
 *    this momory should be free by user programs. Driver won't take care of
 *    this.
 */
#define FB_IOCTL_FLIP_VID_BUFFER            _IO(FB_IOC_MAGIC, 8)

/* Get the current buffer information. User program could use it to display
 * anything directly. If developer wants to allocate multiple video layers,
 * try to use FB_IOCTL_CREATE_VID_BUFFER  to request a brand new video
 * buffer.
 */
#define FB_IOCTL_GET_BUFF_ADDR              _IO(FB_IOC_MAGIC, 9)

/* Get/Set offset position of screen */
#define FB_IOCTL_SET_VID_OFFSET             _IO(FB_IOC_MAGIC, 10)
#define FB_IOCTL_GET_VID_OFFSET             _IO(FB_IOC_MAGIC, 11)

/* Turn on the memory toggle function to improve the frame rate while playing
 * movie.
 */
#define FB_IOCTL_SET_MEMORY_TOGGLE          _IO(FB_IOC_MAGIC, 12)

/* Turn on the memory toggle function to improve the frame rate while playing
 * movie.
 */
#define FB_IOCTL_SET_COLORKEYnALPHA         _IO(FB_IOC_MAGIC, 13)
#define FB_IOCTL_GET_COLORKEYnALPHA         _IO(FB_IOC_MAGIC, 14)
#define FB_IOCTL_SWITCH_GRA_OVLY            _IO(FB_IOC_MAGIC, 15)
#define FB_IOCTL_SWITCH_VID_OVLY            _IO(FB_IOC_MAGIC, 16)

/* For VPro integration */
#define FB_IOCTL_GET_FREELIST               _IO(FB_IOC_MAGIC, 17)

/* Wait for vsync happen. */
#define FB_IOCTL_WAIT_VSYNC                 _IO(FB_IOC_MAGIC, 18)

/* Get and set the display surface */
#define FB_IOCTL_GET_SURFACE                    _IO(FB_IOC_MAGIC, 22)
#define FB_IOCTL_SET_SURFACE                    _IO(FB_IOC_MAGIC, 23)

/* Global alpha blend controls - Maintaining compatibility with existing
   user programs. */
#define FBIOPUT_VIDEO_ALPHABLEND            0xeb
#define FBIOPUT_GLOBAL_ALPHABLEND           0xe1
#define FBIOPUT_GRAPHIC_ALPHABLEND          0xe2

/* color swapping */
#define FBIOPUT_SWAP_GRAPHIC_RED_BLUE       0xe3
#define FBIOPUT_SWAP_GRAPHIC_U_V            0xe4
#define FBIOPUT_SWAP_GRAPHIC_Y_UV           0xe5
#define FBIOPUT_SWAP_VIDEO_RED_BLUE         0xe6
#define FBIOPUT_SWAP_VIDEO_U_V              0xe7
#define FBIOPUT_SWAP_VIDEO_Y_UV             0xe8

/* colorkey compatibility */
#define FBIOGET_CHROMAKEYS                  0xe9
#define FBIOPUT_CHROMAKEYS                  0xea


#define FB_VMODE_RGB565			0x100
#define FB_VMODE_BGR565                 0x101
#define FB_VMODE_RGB1555		0x102
#define FB_VMODE_BGR1555                0x103
#define FB_VMODE_RGB888PACK		0x104
#define FB_VMODE_BGR888PACK		0x105
#define FB_VMODE_RGB888UNPACK		0x106
#define FB_VMODE_BGR888UNPACK		0x107
#define FB_VMODE_RGBA888		0x108
#define FB_VMODE_BGRA888		0x109

#define FB_VMODE_YUV422PACKED               0x0
#define FB_VMODE_YUV422PACKED_SWAPUV        0x1
#define FB_VMODE_YUV422PACKED_SWAPYUorV     0x2
#define FB_VMODE_YUV422PLANAR               0x3
#define FB_VMODE_YUV422PLANAR_SWAPUV        0x4
#define FB_VMODE_YUV422PLANAR_SWAPYUorV     0x5
#define FB_VMODE_YUV420PLANAR               0x6
#define FB_VMODE_YUV420PLANAR_SWAPUV        0x7
#define FB_VMODE_YUV420PLANAR_SWAPYUorV     0x8

#define FB_HWCMODE_1BITMODE                 0x0
#define FB_HWCMODE_2BITMODE                 0x1

#define FB_DISABLE_COLORKEY_MODE            0x0
#define FB_ENABLE_Y_COLORKEY_MODE           0x1
#define FB_ENABLE_U_COLORKEY_MODE           0x2
#define FB_ENABLE_V_COLORKEY_MODE           0x4
#define FB_ENABLE_RGB_COLORKEY_MODE         0x3
#define FB_ENABLE_R_COLORKEY_MODE           0x5
#define FB_ENABLE_G_COLORKEY_MODE           0x6
#define FB_ENABLE_B_COLORKEY_MODE           0x7

#define FB_SYNC_COLORKEY_TO_CHROMA          1
#define FB_SYNC_CHROMA_TO_COLORKEY          2

#define PXA168FB_FB_NUM           2
/* ---------------------------------------------- */
/*              Data Structure                    */
/* ---------------------------------------------- */
/*
 * The follow structures are used to pass data from
 * user space into the kernel for the creation of
 * overlay surfaces and setting the video mode.
 */

#define FBVideoMode unsigned int

struct _sViewPortInfo {
	unsigned short srcWidth;        /* video source size */
	unsigned short srcHeight;
	unsigned short zoomXSize;       /* size after zooming */
	unsigned short zoomYSize;
	unsigned short ycPitch;
	unsigned short uvPitch;
	unsigned int rotation;
	unsigned int yuv_format;
};

struct _sViewPortOffset {
	unsigned short xOffset;         /* position on screen */
	unsigned short yOffset;
};

struct _sVideoBufferAddr {
	unsigned char   frameID;        /* which frame wants */
	unsigned char *startAddr;       /* new buffer (PA) */
	unsigned char *inputData;       /* input buf address (VA) */
	unsigned int length;            /* input data's length */
};

/* The pxa168_fb_chroma structure is here for legacy compatibility with former
 * PXA display driver usage.
 */

struct pxa168_fb_chroma {
	u_char     mode;
	u_char     y_alpha;
	u_char     y;
	u_char     y1;
	u_char     y2;
	u_char     u_alpha;
	u_char     u;
	u_char     u1;
	u_char     u2;
	u_char     v_alpha;
	u_char     v;
	u_char     v1;
	u_char     v2;
};

struct _sColorKeyNAlpha {
	unsigned int win_alpha_en;
	unsigned int alpha_method;
	unsigned int default_alpha_val;
	unsigned int color_match;
};

struct _sOvlySurface {
	FBVideoMode videoMode;
	struct _sViewPortInfo viewPortInfo;
	struct _sViewPortOffset viewPortOffset;
	struct _sVideoBufferAddr videoBufferAddr;
	unsigned int user_data[4];
};

struct _sCursorConfig {
	unsigned char   enable;         /* enable cursor or not */
	unsigned char   mode;           /* 1bit or 2bit mode */
	unsigned int color1;            /* foreground color */
	unsigned int color2;            /* background color */
	unsigned short xoffset;
	unsigned short yoffset;
	unsigned short  width;
	unsigned short height;
	unsigned char *pBuffer;         /* cursor data */
};


typedef enum
{
	LCD_Controller_Enable = 0,
	LCD_Controller_Disable,
	LCD_Controller_Quick_Disable,
}LCD_Controller_Enable_Disable;

typedef enum
{
	LCD_Controller_Active = 0,
	LCD_Controller_Smart,
	LCD_Controller_TV,
}LCD_Controller_Display_Device;

typedef enum
{
	LCD_Controller_Base_Plane = 0,
	LCD_Controller_Overlay1,
	LCD_Controller_Overlay2,
	LCD_Controller_Overlay3,
	LCD_Controller_Cursor,
}LCD_Controller_Overlay;

typedef enum
{
	PIX_FMTOUT_8_R8_G8_B8 = 0,
	PIX_FMTOUT_8_R5G3_G5B3_B5R3,
	PIX_FMTOUT_16_RGB565,
	PIX_FMTOUT_16_R8G8_B8R8_G8B8,
	PIX_FMTOUT_18_RGB666,
	PIX_FMTOUT_24_RGB888,
	PIX_FMTOUT_8_R5G3_G3R5,
}LCD_Controller_OP_FOR;

typedef enum
{
	PIX_FMTIN_RGB_16 = 0,
	PIX_FMTIN_RGB_24,
	PIX_FMTIN_RGB_32,
	PIX_FMTIN_RGB_24_PACK,
	PIX_FMTIN_YUV420,
	PIX_FMTIN_YUV422,
	PIX_FMTIN_YUV444,
	PIX_FMTIN_YUV422IL,
}LCD_Controller_SRC_FOR;

#define PIX_FMT_PSEUDOCOLOR    20

typedef enum
{
	LCD_Controller_104 = 0,
	LCD_Controller_156,
	LCD_Controller_208,
}LCD_Controller_PCLK;

typedef enum
{
	LCD_Controller_ALL_BLOCKS = 0,
	LCD_Controller_FETCH_BLOCK,
	LCD_Controller_MIXER0 = 0x8,
	LCD_Controller_MIXER1,
	LCD_Controller_P_CONVERTER0 = 0xC,
	LCD_Controller_S_CONVERTER0,
	LCD_Controller_S_CONVERTER1,
	LCD_Controller_WGe0 = 0x10,
	LCD_Controller_WGe1,
	LCD_Controller_WGe2,
	LCD_Controller_WGe3,
	LCD_Controller_WGe4,
	LCD_Controller_WGe_crsr0,
	LCD_Controller_WGe_SINK = 0x1B,
	LCD_Controller_NULL = 0x1F,
}LCD_Controller_CLIENT_ID;

typedef enum
{
	LCD_Controller_PARALLEL = 0,
	LCD_Controller_DSI0,
	LCD_Controller_DSI1,
}LCD_Controller_Converter;

typedef enum
{
	LCD_Controller_FIRST_MIXER = 0,
	LCD_Controller_SECOND_MIXER,
	LCD_Controller_THIRD_MIXER,
}LCD_Controller_Mixer;
typedef enum
{
	LCD_Controller_DSI_52MHZ = 0,
	LCD_Controller_DSI_104MHZ,
	LCD_Controller_DSI_156MHZ,
	LCD_Controller_DSI_208MHZ,
	LCD_Controller_DSI_312MHZ,
	LCD_Controller_DSI_UNDEFINEDMHZ,
	LCD_Controller_DSI_624MHZ,

}LCD_Controller_DSI_Clock;

typedef enum{
	LCD_MIXER_DISABLE = 0,
	LCD_M2PARALELL_CONVERTER,
	LCD_M2DSI0,
	LCD_M2DSI1,
}pxa950fb_mixer_output_port;

typedef struct
{
	unsigned int   LCD_FRAME_BUFFER;         /* Frame buffer in memory */
	unsigned short    LCD_Horizontal;           /* Horizontal Dimention */
	unsigned short    LCD_Vertical;             /* Vertical Dimention */
	unsigned short    LCD_Position_x;           /* x position */
	unsigned short    LCD_Position_y;           /* y position */
	unsigned char     LCD_HSW;
	unsigned char     LCD_VSW;
	unsigned char     LCD_BLW;
	unsigned char     LCD_ELW;
	unsigned char     LCD_BFW;
	unsigned char     LCD_EFW;
	unsigned char	  LCD_FRAME_Rate;
	unsigned char     LCD_PCLK;
	unsigned char     LCD_BPP;                  /* Bytes per pixel */
	pxa950fb_mixer_output_port LCD_Mixer[2];
	LCD_Controller_Display_Device LCD_Display;
	LCD_Controller_Overlay LCD_Overlay;
	LCD_Controller_OP_FOR LCD_OP_FOR;
	LCD_Controller_SRC_FOR LCD_SRC_FOR;
	LCD_Controller_CLIENT_ID BP_ID;
	LCD_Controller_CLIENT_ID OL1_ID;
	LCD_Controller_CLIENT_ID OL2_ID;
	LCD_Controller_CLIENT_ID OL3_ID;
}LCD_Controller_Configurations;

/* DSI defination */

#define MVED_DADR_ADDRESS      0x56100210L
#define MVED_DCSR_ADDRESS      0x56100004L
#define MVED_DRCMR2_ADDRESS    0x56100110L
#define MVED_DRCMR3_ADDRESS    0x56100114L


#define DSI_COMMAND_MODE 0
#define DSI_VIDEO_MODE 1
#define DSI_VIDEO_MODE_NON_BURST 2

#define DSI_DISABLE 0
#define DSI_ENABLE 1

#define DSI_NOP_FACTOR 16 //must be 2^DSI_DLY_FACTOR
#define DSI_DLY_FACTOR 4
#define DSI_DCS_DLY 14

#define DSI_BLANK_MLT 3
#if 0
typedef struct
{
	u32     DSI_HSExitTime_nsec;
	u32     DSI_SingleNOP_nsec;
	u32     DSI_HSEntryTime_nsec;
	u16    DSI_BitClock;
	u16    DSI_LPClock;
	u16    DSI_HSExitTime_DSIClocks;
	u8     DSI_HSPREP;
	u8     DSI_HSTRAIL;
	u8     DSI_HSZERO;
	u8     DSI_HSEXIT;
	u8     DSI_LPX;
	u8     DSI_CLZERO;
	u8     DSI_CLTRAIL;
	u8     DSI_HSExitTime_LPClocks;
	u8     DSI_SingleNOP_LPClocks;
	u8     DSI_SingleNOP_DSIClocks;
	u8     DSI_HSEntryTime_LPClocks;
	u8     DSI_HSEntryTime_DSIClocks;
}DSI_Controller_Configurations;

typedef struct
{
	u32     FRAME_HSYNC_freq;
	u32     FRAME_HSYNC_nsec;
	u32     FRAME_HS_bit_time;
	u32     FRAME_pixel_time;
	u32     FRAME_bytes;
	u32    FRAME_bpf;
	u32    FRAME_bps;
	u32    FRAME_pps;
	u8     FRAME_nol;
	u8     FRAME_bpp;
}FRAME_Controller_Configurations;
#endif

typedef enum
{
	LCD_Controller_DSI_1LANE=0,
	LCD_Controller_DSI_2LANE,
	LCD_Controller_DSI_3LANE,
	LCD_Controller_DSI_4LANE,
}LCD_Controller_DSI_DATA_LANES;

typedef enum
{
	LCD_Controller_DSI_RIHS=1,
	LCD_Controller_DSI_ULPS=2,
	LCD_Controller_DSI_LPDT=4,
	LCD_Controller_DSI_BTA=8,
}LCD_Controller_DSI_POWER_MODE;

typedef enum
{
	LCD_Controller_READ_STATUS=0,
	LCD_Controller_AUTO_STATUS_CHECK,
	LCD_Controller_WAIT_FOR_BUSY,
	LCD_Controller_READ_FRAME_BUFFER,
	LCD_Controller_LOAD_MATCH_REGISTER,
	LCD_Controller_COMMAND_WRITE,
	LCD_Controller_DATA_WRITE,
	LCD_Controller_LINE_DATA_WRITE,
	LCD_Controller_WAIT_FOR_VSYNC,
	LCD_Controller_SET_DLY_MULT,
	LCD_Controller_NO_OPERATION,
	LCD_Controller_INTERRUPT_THE_PROCESSOR,
	LCD_Controller_SET_GPIO,
	LCD_Controller_EXECUTE_LOOP_BUFFER=0x10,
	LCD_Controller_FLUSH_LOOP_BUFFER,
	LCD_Controller_START_LABEL,
	LCD_Controller_GOTO_START,
	LCD_Controller_DISABLE_OUTPUT=0x1D,
}LCD_Controller_DSI_COMMAND;

typedef enum
{
	LCD_Controller_VSYNC_START=0x1,
	LCD_Controller_DCS_SHORT_WRITE_NO_PARAMETER=0x05,
	LCD_Controller_DCS_READ_NO_PARAMETER=0x06,
	LCD_Controller_END_OF_TRANSMITION=0x08,
	LCD_Controller_NULL_PACKET=0x09,
	LCD_Controller_RGB_565_PACKET=0x0E,
	LCD_Controller_VSYNC_END=0x11,
	LCD_Controller_DCS_SHORT_WRITE_WITH_PARAMETER=0x15,
	LCD_Controller_BLANKING_PACKET=0x19,
	LCD_Controller_RGB_666_PACKET=0x1E,
	LCD_Controller_HSYNC_START=0x21,
	LCD_Controller_GENERIC_SHORT_WRITE_TWO_PARAMETERS=0x23,
	LCD_Controller_GENERIC_LONG_WRITE=0x29,
	LCD_Controller_RGB_666_LOOSELY_PACKET=0x2E,
	LCD_Controller_HSYNC_END=0x31,
	LCD_Controller_TURN_ON_PERIPHERAL=0x32,
	LCD_Controller_SET_MAXIMUM_RETURN_PACKET_SIZE=0x37,
	LCD_Controller_DCS_LONG_WRITE=0x39,
	LCD_Controller_RGB_888_PACKET=0x3E,
	LCD_Controller_LONG_PACKET_TBD=0x69,
}LCD_Controller_DSI_DATA;

typedef enum
{
	LCD_Controller_DCS_NOP=0x0,
	LCD_Controller_DCS_SOFT_RESET=0x01,
	LCD_Controller_DCS_GET_DIAGNOSTIC_RESULT=0x0F,
	LCD_Controller_DCS_EXIT_SLEEP_MODE=0x11,
	LCD_Controller_DCS_SET_DISPLAY_ON=0x29,
	LCD_Controller_DCS_SET_COLUMN_ADDRESS=0x2A,
	LCD_Controller_DCS_SET_PAGE_ADDRESS=0x2B,
	LCD_Controller_DCS_WRITE_MEMORY_START=0x2C,
	LCD_Controller_DCS_SET_TEAR_ON=0x35,
	LCD_Controller_DCS_GET_PIXEL_FORMAT=0x3A,
	LCD_Controller_DCS_WRITE_MEMORY_CONTINUE=0x3C,
	LCD_Controller_DCS_SET_THSSI_ON=0x81,
	LCD_Controller_DCS_DISPLAY_BUFFER_IO_CONTROL=0x82,
	LCD_Controller_DCS_SET_OUTPUT_VERTICAL_TIMINGS=0x8b,
	LCD_Controller_DCS_SET_OUTPUT_HORIZONTAL_TIMINGS=0x92,
	LCD_Controller_DCS_ENABLE_SET_SPECIAL_COMMAND=0x9D,
	LCD_Controller_DCS_SET_OUTPUT_PIXEL_CLOCK_FREQUENCY=0x9E,
	LCD_Controller_DCS_WRITE_EDISCO_REGISTER=0xFD,
}LCD_Controller_DCS_COMMANDS;

typedef enum
{
	DSI_COMMANDS_FREQ=0x0,
	DSI_COMMANDS_POWER,
	DSI_COMMANDS_NOL,
	DSI_COMMANDS_FIFO_COMMAND,
	DSI_COMMANDS_DSI_COMMAND,
	DSI_COMMANDS_DCS_SHORT_WRITE_NO_PARAMETER,
	DSI_COMMANDS_DCS_SHORT_WRITE_WITH_PARAMETER,
	DSI_COMMANDS_DCS_LONG_WRITE,
	DSI_COMMANDS_INIT_BOARD,
	DSI_COMMANDS_SEND_FRAME,
	DSI_COMMANDS_BER_TEST,
}DSI_COMMANDS;

typedef enum
{
	DSI_BOARD_ZERO=0x0,
	DSI_BOARD_ONE,
	DSI_BOARD_TWO,
	DSI_BOARD_THREE,
	DSI_BOARD_FOUR,
	DSI_BOARD_FIVE,
	DSI_BOARD_SIX,
	DSI_BOARD_SEVEN,
}DSI_BOARD;

#define DSI_BTA_TRIG   (0x1u<<30)
#define DSI_BTA_ACK    (0x1u<<31)

#define BOARD_INIT_MAX_DATA_BYTES   35


/* default fb buffer size WVGA-32bits */
#define DEFAULT_FB_SIZE	(640 * 480 * 4 + PAGE_SIZE)


/* just copied from PXA910 registers */

#ifdef __KERNEL__
#include <linux/interrupt.h>

/*
 * PXA LCD controller private state.
 */
struct pxa950fb_info {
	struct device		*dev;
	struct clk		*clk_lcd;
	struct clk		*clk_axi;
	struct clk		*clk_dsi0;
	struct clk		*clk_dsi1;
	int                     id;
	void			*reg_base;
	unsigned long		new_addr;
	dma_addr_t		fb_start_dma;
	void			*fb_start;
	int			fb_size;
	atomic_t		w_intr;
	wait_queue_head_t	w_intr_wq;
	struct mutex		access_ok;
	struct _sOvlySurface    surface;
	struct _sColorKeyNAlpha ckey_alpha;
	struct fb_videomode	dft_vmode;
	struct fb_videomode     out_vmode;
	int                     fixed_output;
	unsigned char		*hwc_buf;
	unsigned int		pseudo_palette[16];
	struct tasklet_struct	tasklet;
	char 			*mode_option;
	struct fb_info          *fb_info;
	int                     io_pin_allocation;
	int			pix_fmt;
	unsigned		is_blanked:1;
	unsigned                edid:1;
	unsigned                cursor_enabled:1;
	unsigned                cursor_cfg:1;
	unsigned		panel_rbswap:1;
	unsigned		debug:1;
	unsigned		active:1;
	unsigned                enabled:1;
	unsigned                edid_en:1;

	/*
	 * 0: DMA mem is from DMA region.
	 * 1: DMA mem is from normal region.
	 */
	unsigned                mem_status:1;
};

/*
 * PXA fb machine information
 */
struct pxa950fb_mach_info {
	char	id[16];
	unsigned int	sclk_clock;

	int		num_modes;
	struct fb_videomode *modes;

	/*
	 * Pix_fmt
	 */
	unsigned	pix_fmt;

	unsigned	pix_fmt_in;	/* PXA950 */
	unsigned	pix_fmt_out;

	/*
	 * I/O pin allocation.
	 */
	unsigned	io_pin_allocation_mode:4;

	/*
	 * Dumb panel -- assignment of R/G/B component info to the 24
	 * available external data lanes.
	 */
	unsigned	dumb_mode:4;
	unsigned	panel_type:2;	/* PXA950 */
	unsigned	panel_rgb_reverse_lanes:1;

	/*
	 * Dumb panel -- GPIO output data.
	 */
	unsigned	gpio_output_mask:8;
	unsigned	gpio_output_data:8;

	/* PXA950 mixer configure */
	pxa950fb_mixer_output_port      mixer0;
	pxa950fb_mixer_output_port      mixer1;

	/*
	 * Dumb panel -- configurable output signal polarity.
	 */
	unsigned	invert_composite_blank:1;
	unsigned	invert_pix_val_ena:1;
	unsigned	invert_pixclock:1;
	unsigned	invert_vsync:1;
	unsigned	invert_hsync:1;
	unsigned	panel_rbswap:1;
	unsigned	active:1;
	unsigned	enable_lcd:1;
	/*
	 * SPI control
	 */
	unsigned int	spi_ctrl;
	unsigned int	spi_gpio_cs;
	unsigned int 	spi_gpio_reset;
	/*
	 * power on/off function.
	 */
	void (*pxa950fb_lcd_power)(int);
	void (*pxa950fb_dsi_reset)(void);
};
extern void pxa950fb_spi_send(struct pxa950fb_info *fbi, u16 *cmd, int count, unsigned int spi_gpio_cs);
void set_pxa950_fb_info(struct pxa950fb_mach_info *info);
void set_pxa950_fb_ovly_info(struct pxa950fb_mach_info *info);
#endif /* __KERNEL__ */
#endif /* __ASM_MACH_PXA950FB_H */
