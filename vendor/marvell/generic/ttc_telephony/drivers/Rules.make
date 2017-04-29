

#*****************************************************************************
# BEGIN SECTION (Define)

# Define directory
# 

#Project root

PXA_ROOT_DIR		:= $(PWD)
CONFIG_CC_DEVELOPMENT=n


#Project source 
PXA_SRC_DIR		:= $(PXA_ROOT_DIR)

DUMMY			:= $(shell if [ ! -e $(PXA_SRC_DIR)/.config ];then touch $(PXA_SRC_DIR)/.config;fi;)
include $(PXA_SRC_DIR)/.config

PXA_SRC_PVK_DIR		:= $(PXA_SRC_DIR)/preview-kit
PXA_SRC_PVK_LNX_DIR	:= $(PXA_ROOT_DIR)/../../../../../kernel/kernel/
PXA_SRC_ANDROID_DEVICE_DIR	:= $(PXA_SRC_DIR)/../../../../../
PXA_SRC_IPP_DIR		:= $(PXA_SRC_DIR)/ipp
PXA_SRC_GST_DIR		:= $(PXA_SRC_DIR)/gstreamer
PXA_SRC_UM_DIR		:= $(PXA_SRC_DIR)/usage-model
PXA_SRC_GFX_DIR		:= $(PXA_SRC_DIR)/graphics
PXA_SRC_MPDC_DIR	:= $(PXA_SRC_DIR)/mpdc
PXA_SRC_XDB_DIR	:= $(PXA_SRC_DIR)/xdb

ifeq "$(CONFIG_CPU_MHNL)" "y"
WTPTP_DIR_SUFFIX=pxa300
endif

ifeq "$(CONFIG_CPU_MHNLV)" "y"
WTPTP_DIR_SUFFIX=pxa310
endif

ifeq "$(CONFIG_CPU_TAVOR)" "y"
WTPTP_DIR_SUFFIX=pxa930
endif
PXA_SRC_WTPTP_DIR	:= $(PXA_SRC_DIR)/wtptp_$(WTPTP_DIR_SUFFIX)

ifeq "$(CONFIG_GEN_DEBUG)" "y"
DBG_REL_VER		:= dbg
else
DBG_REL_VER		:= rel
endif

#Host related dir
ifeq "$(CONFIG_CC_DEVELOPMENT)" "y"
DUMMY			:= $(shell if [ ! -e $$HOME/xscalelinux ];then mkdir -p $$HOME/xscalelinux;\
			cp -fr ../dbg $$HOME/xscalelinux;cp -fr ../rel $$HOME/xscalelinux;fi;)
PXA_HOST_DIR		:= $(HOME)/xscalelinux/$(DBG_REL_VER)/host
else
PXA_HOST_DIR		:= $(PXA_ROOT_DIR)/$(DBG_REL_VER)/host
endif
PXA_HOST_INCLUDE_DIR	:= $(PXA_HOST_DIR)/include
PXA_HOST_LIB_DIR	:= $(PXA_HOST_DIR)/lib

#Target related dir
ifeq "$(CONFIG_CC_DEVELOPMENT)" "y"
PXA_TARGET_DIR		:= $(HOME)/xscalelinux/$(DBG_REL_VER)/target
else
PXA_TARGET_DIR		:= $(PXA_ROOT_DIR)/output
endif
PXA_TARGET_FS_DIR	:= $(PXA_TARGET_DIR)/marvell
PXA_TARGET_MOD_DIR	:= $(PXA_TARGET_FS_DIR)/lib/modules
PXA_TARGET_LIB_DIR	:= $(PXA_TARGET_FS_DIR)/usr/lib
PXA_TARGET_BIN_DIR	:= $(PXA_TARGET_FS_DIR)/usr/bin
PXA_TARGET_IMAGE_DIR	:= $(PXA_TARGET_DIR)/
# END SECTION (Define)


#*****************************************************************************

##############################################################################
# BEGIN SECTION (complier parameter)
#
#	Compiler setting 
#

export ARCH		:= arm
ifeq "$(CONFIG_BIONIC)" "y"
export CROSS_COMPILE    := arm-eabi-
endif
#else
#export CROSS_COMPILE	:= arm-linux-
ifeq "$(CONFIG_GEN_ARM_LINUX)" "y"
export CROSS_COMPILE    := arm-linux-
endif
ifeq "$(CONFIG_GEN_MARVELL_LINUX)" "y"
export CROSS_COMPILE    := arm-marvell-linux-gnueabi-
endif

#
#	Include make variables (CC, etc...)
#
ASM	:= $(CROSS_COMPILE)as
LD	:= $(CROSS_COMPILE)ld
CC	:= $(CROSS_COMPILE)gcc
CPP	:= $(CROSS_COMPILE)c++
AR	:= $(CROSS_COMPILE)ar
STRIP   := $(CROSS_COMPILE)strip
OBJCOPY	:= $(CROSS_COMPILE)objcopy
OBJDUMP	:= $(CROSS_COMPILE)objdump
RANLIB  := $(CROSS_COMPILE)ranlib
CCOMPILER=$(CC)

ifeq "$(CONFIG_BIONIC)" "y"
include $(PXA_SRC_DIR)/Bionic.make
PXA_HOST_INCLUDE_DIR  := $(PXA_SRC_ANDROID_DEVICE_DIR)/external/alsa-lib/include
else
PXA_HOST_INCLUDE_DIR  := $(PXA_SRC_ANDROID_DEVICE_DIR)/external/alsa-lib/include
LIBDIR	:= -L$(PXA_HOST_LIB_DIR) -L$(PXA_TARGET_FS_DIR)/lib
endif

ifeq "$(CONFIG_BIONIC)" "y"
COMMONCFLAGS = $(WARNINGFLAG) -DLINUX -D__linux__ -fno-exceptions -msoft-float -fpic -ffunction-sections \
			   -funwind-tables -fstack-protector -Wa,--noexecstack -fno-short-enums \
			   -march=armv5te -mtune=xscale -mthumb-interwork -fmessage-length=0 -finline-functions \
			   -fno-inline-functions-called-once -fgcse-after-reload -frerun-cse-after-loop -frename-registers \
			   -mthumb -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -g
else
COMMONCFLAGS = $(WARNINGFLAG) -mcpu=iwmmxt -mtune=iwmmxt -mabi=aapcs-linux -DLINUX -D__linux__ -fno-exceptions
endif

#
#	Flag declare
#

ifeq "$(CONFIG_BIONIC)" "y"

ifeq "$(CONFIG_GEN_DEBUG)" "y"
WARNINGFLAG	= -Waggregate-return -Wmissing-noreturn -W -Wall
CFLAGS		= -I$(PXA_SRC_PVK_LNX_DIR)/include -I$(PXA_HOST_INCLUDE_DIR) $(LIBDIR) $(BIONICS_INCS) -I$(PXA_SRC_PVK_LNX_DIR)/arch/arm/include\
		  $(COMMONCFLAGS)
AFLAGS 		= -mcpu=iwmmxt
else
WARNINGFLAG	= -Waggregate-return -Wmissing-noreturn -W -Wall
CFLAGS		= -I$(PXA_HOST_INCLUDE_DIR) $(LIBDIR) $(BIONICS_INCS) -I$(PXA_SRC_PVK_LNX_DIR)/include -I$(PXA_SRC_PVK_LNX_DIR)/arch/arm/include\
		  $(COMMONCFLAGS) 

AFLAGS 		= -mcpu=iwmmxt
#WARNINGFLAG	= -W 
#CFLAGS		= -I$(PXA_SRC_PVK_LNX_DIR)/include -I$(PXA_HOST_INCLUDE_DIR) $(LIBDIR)\
#		  -O2 $(WARNINGFLAG) -mcpu=iwmmxt -mtune=iwmmxt -mabi=aapcs-linux
#AFLAGS		= -mcpu=iwmmxt
endif

CFLAGS+=-DBIONIC


else

ifeq "$(CONFIG_GEN_DEBUG)" "y"
WARNINGFLAG	= -Waggregate-return -Wmissing-noreturn -W -Wall
CFLAGS		= -I$(PXA_SRC_PVK_LNX_DIR)/include -I$(PXA_HOST_INCLUDE_DIR) $(LIBDIR)\
		  -g $(WARNINGFLAG) -mcpu=iwmmxt -mtune=iwmmxt -mabi=aapcs-linux
AFLAGS 		= -mcpu=iwmmxt
else
WARNINGFLAG	= -Waggregate-return -Wmissing-noreturn -W -Wall
CFLAGS		= -I$(PXA_SRC_PVK_LNX_DIR)/include -I$(PXA_HOST_INCLUDE_DIR) $(LIBDIR)\
		  -g $(WARNINGFLAG) -mcpu=iwmmxt -mtune=iwmmxt -mabi=aapcs-linux
AFLAGS 		= -mcpu=iwmmxt
#WARNINGFLAG	= -W 
#CFLAGS		= -I$(PXA_SRC_PVK_LNX_DIR)/include -I$(PXA_HOST_INCLUDE_DIR) $(LIBDIR)\
#		  -O2 $(WARNINGFLAG) -mcpu=iwmmxt -mtune=iwmmxt -mabi=aapcs-linux
#AFLAGS		= -mcpu=iwmmxt
endif

endif

CFLAGS+=-D_POSIX_C_SOURCE

ifeq "$(CONFIG_INSTRUMENT)" "y"
CFLAGS+=-finstrument-functions -rdynamic
endif

CFLAGS+=-DLOAD_SEAGULL_ADDR=$(SEAGULL_ADDR)\
		-DLOAD_MSA_ADDR=$(MSA_ADDR)\
		-DLOAD_RDATA_ADDR=$(RDATA_ADDR)
.SUFFIXES : .o .S .s .cpp .c .i

.S.o :
	$(ASM) $(AFLAGS) -o $@ $<

.s.o :
	$(ASM) $(AFLAGS) -o $@ $<

.cpp.o :
	$(CPP) $(CFLAGS) -c -o $@ $<

.c.o :
	$(CC) $(CFLAGS) -c -o $@ $<

.c.i :
	$(CC) $(CFLAGS) -DDIAG_API_H -C -E -o $@ $<

ifneq ($(shell echo $$UID), 0)
export SUDO=sudo 
else
endif


# END SECTION (complier parameter)
#****************************************************************************
